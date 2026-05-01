#include <Wire.h>

#define SLAVE_ADDR 1//8
#define PLOT 0x09U

String mensagemRecebida = "";
int teste, c;


void setup() {
  // Wire.begin(8); // Inicia o I2C como escravo com endereço 8
  // Wire.begin(SLAVE_ADDR);     // Inicia como SLAVE endereço 8
  Wire.begin(1); // Inicia o I2C como escravo com endereço 8
  Wire.onReceive(receberDados); // Registra a função de callback para recepção
  Wire.onRequest(enviarEvento);
  Serial.begin(9600); // Para depuração
}

void loop() {
  // Nada aqui, a recepção é tratada no callback

}

void receberDados(int numBytes) {
  while (Wire.available()) { // Enquanto houver dados disponíveis
    // char c = Wire.read(); // Lê um byte
    // int c = Wire.read(); // Lê um byte
    c = Wire.read(); // Lê um byte
    Serial.print(c); // Imprime no Serial Monitor
  }
  Serial.println(); // Nova linha após a mensagem completa
}

// Funcao chamada quando o master solicita dados
void enviarEvento() {
  // Wire.write("OK123");   // Envia 5 bytes
    if(c==0xFFu){
      Wire.write(PLOT);   // Envia 5 bytes
    // Wire.onRequest(enviarEvento);
    Serial.print(PLOT); // Imprime no Serial Monitor
    Serial.println(); // Nova linha após a mensagem completa
  }
}

//-----------------------------------------------------------------

// void setup() {
//   Wire.onReceive(receberEvento);
//   Wire.onRequest(enviarEvento);
//   Serial.begin(9600);
// }

// // Função chamada quando recebe dados
// void receberEvento(int bytes) {
//   mensagemRecebida = "";
  
//   while (Wire.available()) {
//     char c = Wire.read();
//     mensagemRecebida += c;
//   }

//   Serial.print("Mensagem recebida: ");
//   Serial.println(mensagemRecebida);
// }

