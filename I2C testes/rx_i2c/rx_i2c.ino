#include <Wire.h>

void setup() {
  // Wire.begin(8); // Inicia o I2C como escravo com endereço 8
  Wire.begin(12); // Inicia o I2C como escravo com endereço 8
  Wire.onReceive(receberDados); // Registra a função de callback para recepção
  Serial.begin(9600); // Para depuração
}

void loop() {
  // Nada aqui, a recepção é tratada no callback
}

void receberDados(int numBytes) {
  while (Wire.available()) { // Enquanto houver dados disponíveis
    // char c = Wire.read(); // Lê um byte
    int c = Wire.read(); // Lê um byte
    Serial.print(c); // Imprime no Serial Monitor
  }
  Serial.println(); // Nova linha após a mensagem completa
}