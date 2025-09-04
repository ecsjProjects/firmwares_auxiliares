#define START_BYTE 0xAA
#define END_BYTE 0x55

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  receivePacket();
}



byte calcCRC(byte *data, byte len) {
  byte crc = 0;
  for (int i = 0; i < len; i++) {
    crc ^= data[i];
  }
  return crc;
}

void receivePacket() {
  static enum { WAIT_START,
                READ_LEN,
                READ_CMD,
                READ_PARAMS,
                READ_CRC,
                READ_ACK,
                WAIT_END } state = WAIT_START;
  static byte packet_len, command, params[20], param_len, crc, ack, bytes_read;

  while (Serial.available()) {
    byte b = Serial.read();

    switch (state) {
      case WAIT_START:
        if (b == START_BYTE) state = READ_LEN;
        break;
      case READ_LEN:
        packet_len = b;
        bytes_read = 0;
        state = READ_CMD;
        break;
      case READ_CMD:
        command = b;
        param_len = packet_len - (1 + 1 + 1 + 1);  // COMMAND, CRC, ACK, END_BYTE
        bytes_read = 0;
        state = param_len > 0 ? READ_PARAMS : READ_CRC;
        break;
      case READ_PARAMS:
        params[bytes_read++] = b;
        if (bytes_read >= param_len) state = READ_CRC;
        break;
      case READ_CRC:
        crc = b;
        state = READ_ACK;
        break;
      case READ_ACK:
        ack = b;
        state = WAIT_END;
        break;
      case WAIT_END:
        if (b == END_BYTE) {
          // Valida CRC
          byte crc_data[1 + param_len];
          crc_data[0] = command;
          for (int i = 0; i < param_len; i++) crc_data[1 + i] = params[i];
          byte calc_crc = calcCRC(crc_data, 1 + param_len);

          if (calc_crc == crc) {
            // Pacote válido!
            // Processa comando, parâmetros, ACK
            Serial.print("Recebido comando: ");
            Serial.println(command);
            // Serial.print("ACK: ");
            // Serial.println(ack);
            Serial.print("Parametros: ");
            for (int i = 0; i < param_len; i++) {
              Serial.print(params[i]);
              Serial.print(' ');
            }
            Serial.println();
          } else {
            Serial.println("CRC inválido!");
          }
        }
        state = WAIT_START;
        break;
    }
  }
}