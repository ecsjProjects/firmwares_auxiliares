// Definição dos pinos onde os 8 botões estão conectados
// const int pinosBotoes[8] = {2, 3, 4, 5, 6, 7, 8, 9};
const int pinosBotoes[8] = {0, 1, 2, 3, 4, 5, 6, 7};

// Arrays para armazenar o estado atual e o estado anterior de cada botão
bool estadoAnterior[8] = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH};
bool estadoAtual[8]    = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH, HIGH};

void setup() {
  Serial.begin(9600);
  
  // Configura todos os 8 pinos como entrada com o Pull-up interno ativado
  for (int i = 0; i < 8; i++) {
    // pinMode(pinosBotoes[i], INPUT_PULLUP);
    pinMode(pinosBotoes[i], INPUT);
  }
  
  Serial.println("Sistema Iniciado. Aguardando clique dos botoes...");
}

void loop() {
  // Passa verificando cada um dos 8 botões
  for (int i = 0; i < 8; i++) {
    
    // Lê o estado atual do botão 'i'
    estadoAtual[i] = digitalRead(pinosBotoes[i]);
    
    // LOGICA PRINCIPAL:
    // Se o estado anterior era LOW (pressionado) e o atual é HIGH (solto)
    if (estadoAnterior[i] == LOW && estadoAtual[i] == HIGH) {
      
      // O botão foi pressionado e solto! Coloque sua ação aqui:
      Serial.print("Botao ");
      Serial.print(i + 1); // Mostra de 1 a 8 na tela
      Serial.println(" foi pressionado e SOLTO!");
      
      // Pequeno delay para evitar o "bouncing" (ruído elétrico do botão)
      delay(50); 
    }
    
    // Atualiza o estado anterior com o valor do estado atual para a próxima leitura
    estadoAnterior[i] = estadoAtual[i];
  }
}