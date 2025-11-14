#define UART_BUF_SIZE 256

// ---- Protocol control bytes (você precisa definir conforme seu protocolo)
#define DLE 0x10
#define STX 0x02
#define ETX 0x03

typedef enum {
  VALIDITY_VALID,
  VALIDITY_INVALID_LENGTH,
  VALIDITY_INVALID_CRC
} validity_t;

typedef struct {
  uint16_t payload_len;
  uint8_t seq_num;
  uint8_t op_code;
  uint8_t *payload;
  validity_t validity;
} command_t;

// ---- Protótipos
bool data_to_command(uint8_t *data, size_t len, command_t *command);
void onCommandReceived(command_t *cmd);

// ---- Buffer e estados
static uint8_t buffer[UART_BUF_SIZE];
static size_t indexBuf = 0;
static bool escape = false;
static bool start_found = false;

// ---- Exemplo simples de CRC16 (igual ao que você usa se já tiver implementado)
uint16_t calculate_crc16(uint16_t crc, uint8_t *data, size_t len) {
  for (size_t i = 0; i < len; i++) {
    crc ^= (uint16_t)data[i] << 8;
    for (uint8_t j = 0; j < 8; j++) {
      if (crc & 0x8000) crc = (crc << 1) ^ 0x1021;
      else crc <<= 1;
    }
  }
  return crc;
}

// ---- (Se você tiver essa função original, mantenha. Caso não, deixo como stub)
void unstuff_data_in_place(uint8_t *buf, size_t bufsize, size_t start, size_t end, size_t *len) {
  // Se seu protocolo usa byte stuffing, implemente aqui.
  // Se não usa, pode remover a chamada.
  *len = end;
}

void setup() {
  Serial.begin(9600);
  Serial.println("[Arduino UNO] UART Receiver Ready!");
}

void loop() {
  receive_commands();
}

void receive_commands() {

  while (Serial.available()) {
    uint8_t byteIn = Serial.read();

    if (!escape && byteIn == DLE) {
      escape = true;
      continue;
    }

    if (escape) {
      escape = false;

      if (byteIn == STX) {
        // Start of packet
        indexBuf = 0;
        start_found = true;
        continue;
      }

      if (byteIn == ETX) {
        // End of packet
        if (start_found) {
          size_t finalLen = indexBuf;
          unstuff_data_in_place(buffer, UART_BUF_SIZE, 0, finalLen, &finalLen);

          command_t cmd;
          data_to_command(buffer, finalLen, &cmd);

          // Aloca payload se necessário
          if (cmd.payload_len > 0) {
            uint8_t *payloadCopy = (uint8_t *)malloc(cmd.payload_len);
            memcpy(payloadCopy, cmd.payload, cmd.payload_len);
            cmd.payload = payloadCopy;
          } else {
            cmd.payload = NULL;
          }

          onCommandReceived(&cmd);

          if (cmd.payload) free(cmd.payload);
        }

        indexBuf = 0;
        start_found = false;
        continue;
      }
    }

    if (start_found && indexBuf < UART_BUF_SIZE) {
      buffer[indexBuf++] = byteIn;
    }
  }
}

bool data_to_command(uint8_t *data, size_t len, command_t *command) {
  if (!command) return false;

  int i = 0;
  command->payload_len = (data[i] << 8) | data[i+1]; i += 2;
  command->seq_num = data[i++];
  command->op_code = data[i++];
  command->payload = &data[i];

  if (len != 4 + (size_t)command->payload_len + 2) {
    command->validity = VALIDITY_INVALID_LENGTH;
    return true;
  }

  i += command->payload_len;
  uint16_t data_crc16 = (data[i] << 8) | data[i+1];

  uint16_t crc16 = 0xFFFF;
  crc16 = calculate_crc16(crc16, data, len - 2);

  command->validity = (crc16 == data_crc16) ? VALIDITY_VALID : VALIDITY_INVALID_CRC;
  return true;
}

void onCommandReceived(command_t *cmd) {
  Serial.println("\n---- COMMAND RECEIVED ----");
  Serial.print("Payload Len: ");
  Serial.println(cmd->payload_len);
  Serial.print("Seq Num: ");
  Serial.println(cmd->seq_num);
  Serial.print("OpCode: ");
  Serial.println(cmd->op_code);
  Serial.print("Validity: ");
  Serial.println(cmd->validity == VALIDITY_VALID ? "OK" : "INVALID");

  if (cmd->payload_len > 0 && cmd->payload) {
    Serial.print("Payload: ");
    for (int i = 0; i < cmd->payload_len; i++) {
      Serial.print(cmd->payload[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
  }
  Serial.println("--------------------------\n");
}
