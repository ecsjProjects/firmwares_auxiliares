#include <stdint.h>

#define ENABLE_LED 0xFFU
#define DESABLE_LED 0x00U

#define DEBOUNCE_TIME 150U
#define TIME_INIT 2000U

#define SHIFT_VALUE 4U

// Rastreio de botoes
enum{
  BOTAO_1 = 0x00U,
  BOTAO_2,
  BOTAO_3,
  BOTAO_4,
  BOTAO_5,
  BOTAO_6,
  BOTAO_7,
  BOTAO_8,
  DATA_EXP2,
  CLOCK_PIN2,
  DATA_EXP1,
  CLOCK_PIN1,
  LATCH_PIN
};

// --- Variáveis de Controle ---
byte estadoBotoesAnt[8] = { HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH };
byte estadoLedsExp1 = ENABLE_LED;  // Armazena o estado dos LEDs do Expander 1 (em bits)
byte estadoLedsExp2 = ENABLE_LED;  // Armazena o estado dos LEDs do Expander 2 (em bits)

// Tempo para debounce simples
unsigned long ultimoDebounce = 0x00U;

void setup() {
  // Configura os pinos 0 a 7 como ENTRADA
  // (O botão deve mandar GND/0V quando pressionado)
  for (uint8_t i = BOTAO_1; i <= BOTAO_8; i++) {
    pinMode(i, INPUT);
  }

  // Configura os pinos de saída para os registradores de deslocamento
  pinMode(LATCH_PIN, OUTPUT);
  pinMode(CLOCK_PIN1, OUTPUT);
  pinMode(CLOCK_PIN2, OUTPUT);
  pinMode(DATA_EXP1, OUTPUT);
  pinMode(DATA_EXP2, OUTPUT);

  // Inicializa os LEDs desligados
  atualizarExpanders();

  delay(TIME_INIT);

  estadoLedsExp1 = DESABLE_LED;
  estadoLedsExp2 = DESABLE_LED;
  atualizarExpanders();
}

void loop() {
  // Debounce não-bloqueante
  if ((millis() - ultimoDebounce) > DEBOUNCE_TIME) {

    // Varre os 8 botões (pinos 0 a 7)
    for (uint8_t i = BOTAO_1; i <= BOTAO_8; i++) {
      uint8_t leitura = digitalRead(i);

      // Detecta a transição: Estava pressionado (LOW) e foi SOLTADO (HIGH)
      if (leitura == HIGH && estadoBotoesAnt[i] == LOW) {

         if (i < BOTAO_5) {
          // Botões 0 a 3 (1 a 4 físicos) mudam o Expander 1
          // Inverte o bit correspondente usando XOR (^)
          estadoLedsExp2 ^= (1U << i);
        } else {
          // Botões 4 a 7 (5 a 8 físicos) mudam o Expander 2
          // i - 4 ajusta a posição do bit para os pinos 0 a 3 do segundo CI
          estadoLedsExp1 ^= (1U << (i - SHIFT_VALUE));
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