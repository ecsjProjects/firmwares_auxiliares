
// --- Definição dos Pinos dos Expanders ---
const int LATCH_PIN = 12;  // Pino ST_CP dos dois CIs
const int CLOCK_PIN1 = 11;  // Pino SH_CP dos dois CIs
const int DATA_EXP1 = 10;  // Pino DS do Expander 1 (LEDs 1 a 4)
const int DATA_EXP2 = 8;   //9;  // Pino DS do Expander 2 (LEDs 5 a 8)
const int CLOCK_PIN2 = 9;  //8; // Pino SH_CP dos dois CIs

// --- Variáveis de Controle ---
byte estadoBotoesAnt[8] = { HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH };
byte estadoLedsExp1 = 0x00;  // Armazena o estado dos LEDs do Expander 1 (em bits)
byte estadoLedsExp2 = 0x00;  // Armazena o estado dos LEDs do Expander 2 (em bits)

// Tempo para debounce simples
unsigned long ultimoDebounce = 0;
const int tempoDebounce = 150;  //50;

void setup() {
  // Configura os pinos 0 a 7 como ENTRADA com Pull-up interno
  // (O botão deve mandar GND/0V quando pressionado)
  for (int i = 0; i <= 7; i++) {
    pinMode(i, INPUT);
    // Serial.begin(9600);
  }

  // Configura os pinos de saída para os registradores de deslocamento
  pinMode(LATCH_PIN, OUTPUT);
  pinMode(CLOCK_PIN1, OUTPUT);
  pinMode(CLOCK_PIN2, OUTPUT);
  pinMode(DATA_EXP1, OUTPUT);
  pinMode(DATA_EXP2, OUTPUT);

  // Inicializa os LEDs desligados
  atualizarExpanders();
}

void loop() {
  // Debounce não-bloqueante
  if ((millis() - ultimoDebounce) > tempoDebounce) {

    // Varre os 8 botões (pinos 0 a 7)
    for (int i = 0; i < 8; i++) {
      int leitura = digitalRead(i);

      // Detecta a transição: Estava pressionado (LOW) e foi SOLTADO (HIGH)
      if (leitura == HIGH && estadoBotoesAnt[i] == LOW) {

         if (i < 4) {
          // Botões 0 a 3 (1 a 4 físicos) mudam o Expander 1
          // Inverte o bit correspondente usando XOR (^)
          // estadoLedsExp1 ^= (1 << i);
          estadoLedsExp2 ^= (1 << i);
          // Serial.println("Botões HA");

        } else {
          // Botões 4 a 7 (5 a 8 físicos) mudam o Expander 2
          // i - 4 ajusta a posição do bit para os pinos 0 a 3 do segundo CI
          // estadoLedsExp2 ^= (1 << (i - 4));
          estadoLedsExp1 ^= (1 << (i - 4));
          // Serial.println("Botões HB");
        }

        // Atualiza a saída física dos LEDs
        atualizarExpanders();
        ultimoDebounce = millis();
      }

      // Salva o estado atual para a próxima comparação
      estadoBotoesAnt[i] = leitura;
    }
  }
}

// Função responsável por enviar os dados salvos para os CIs
void atualizarExpanders() {
  // Abre o latch para receber dados
  digitalWrite(LATCH_PIN, LOW);

  // Envia os dados para o Expander 1 e Expander 2 simultaneamente
  // Como usamos shiftOut nativo, enviamos um de cada vez nos pinos de dados correspondentes
  shiftOut(DATA_EXP1, CLOCK_PIN1, MSBFIRST, estadoLedsExp1);
  shiftOut(DATA_EXP2, CLOCK_PIN2, MSBFIRST, estadoLedsExp2);

  // Fecha o latch para atualizar as saídas dos LEDs ao mesmo tempo
  digitalWrite(LATCH_PIN, HIGH);
}