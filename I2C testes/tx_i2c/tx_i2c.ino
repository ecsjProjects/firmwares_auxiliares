#include <Wire.h>
int teste;
void setup() {
  Wire.begin();        // Inicia o I2C como mestre
  Serial.begin(9600);  // Para depuração
  pinMode(22, INPUT_PULLUP);
}

void loop() {

  teste = digitalRead(22);
  if (teste == 0) {
    Wire.beginTransmission(8);  // Endereço do escravo (pode mudar para o que quiser)
    // Wire.write("Olá I2C!"); // Envia uma string como exemplo
    Wire.write(10); // Envia uma string como exemplo
    Wire.endTransmission();  // Finaliza a transmissão

    Serial.println("Dados enviados!");
    //delay(1000); // Espera 1 segundo antes de enviar novamente
  }
}