/*********************************************************************
  PCA8565 RTC - Sketch Completo para Arduino (Primeira Inicialização)
  Comunicação I²C
  Autor: Grok (adaptado para você)
*********************************************************************/

#include <Wire.h>

#define PCA8565_ADDR 0x51  // Endereço I2C 7-bit (0xA2 >> 1)


// ====================== FUNÇÕES AUXILIARES ======================
// Converte decimal → BCD (ex: 48 → 0x48)
uint8_t decToBcd(uint8_t val) {
  return ((val / 10 * 16) + (val % 10));
}

// Converte BCD → decimal (ex: 0x48 → 48)
uint8_t bcdToDec(uint8_t val) {
  return ((val / 16 * 10) + (val % 16));
}


// ====================== CONFIGURAÇÃO INICIAL (PRIMEIRA VEZ) ======================
void setPCA8565Time(uint8_t second, uint8_t minute, uint8_t hour,
                    uint8_t day, uint8_t weekday, uint8_t month, uint8_t year) {

  // 1. Para o RTC (STOP = 1)
  Wire.beginTransmission(PCA8565_ADDR);
  Wire.write(0x00);  // Registrador Control_1
  Wire.write(0x20);  // STOP = 1
  Wire.endTransmission();

  // 2. Escreve data/hora completa (7 bytes em sequência)
  Wire.beginTransmission(PCA8565_ADDR);
  Wire.write(0x02);                     // Começa no registrador de segundos
  Wire.write(decToBcd(second) & 0x7F);  // Segundos (VL = 0)
  Wire.write(decToBcd(minute));
  Wire.write(decToBcd(hour));
  Wire.write(decToBcd(day));
  Wire.write(decToBcd(weekday));  // 0=Domingo, 1=Segunda, ..., 6=Sábado
  Wire.write(decToBcd(month));    // Century = 0 (2000-2099)
  Wire.write(decToBcd(year % 100));
  Wire.endTransmission();

  // 3. Inicia o RTC (STOP = 0)
  Wire.beginTransmission(PCA8565_ADDR);
  Wire.write(0x00);
  Wire.write(0x00);  // STOP = 0
  Wire.endTransmission();

  Serial.println("✅ PCA8565 configurado com sucesso!");
}


// ====================== LEITURA DA HORA ======================
void readPCA8565Time() {
  uint8_t second, minute, hour, day, weekday, month, year;

  Wire.beginTransmission(PCA8565_ADDR);
  Wire.write(0x02);  // Começa no registrador 0x02
  Wire.endTransmission();

  Wire.requestFrom(PCA8565_ADDR, 7);  // Lê 7 bytes

  uint8_t sec_reg = Wire.read();
  second = bcdToDec(sec_reg & 0x7F);
  minute = bcdToDec(Wire.read() & 0x7F);
  hour = bcdToDec(Wire.read() & 0x3F);
  day = bcdToDec(Wire.read() & 0x3F);
  weekday = bcdToDec(Wire.read() & 0x07);
  month = bcdToDec(Wire.read() & 0x1F);
  year = bcdToDec(Wire.read());

  // Verifica flag de bateria baixa / primeira energização
  if (sec_reg & 0x80) {
    Serial.println("⚠️  VL = 1 → Primeira energização ou bateria baixa!");
  }

  // Imprime data/hora formatada
  Serial.print("Data/Hora: ");
  if (day < 10) Serial.print("0");
  Serial.print(day);
  Serial.print("/");
  if (month < 10) Serial.print("0");
  Serial.print(month);
  Serial.print("/20");
  if (year < 10) Serial.print("0");
  Serial.print(year);
  Serial.print("  ");

  if (hour < 10) Serial.print("0");
  Serial.print(hour);
  Serial.print(":");
  if (minute < 10) Serial.print("0");
  Serial.print(minute);
  Serial.print(":");
  if (second < 10) Serial.print("0");
  Serial.print(second);

  // Nome do dia da semana (opcional)
  const char* dias[] = { "Dom", "Seg", "Ter", "Qua", "Qui", "Sex", "Sab" };
  Serial.print("  (");
  Serial.print(dias[weekday]);
  Serial.println(")");
}


void setup() {
  Serial.begin(115200);
  Wire.begin();  // Inicia comunicação I2C

  Serial.println("\n=== PCA8565 RTC - Inicialização ===");

  // ====================== PRIMEIRA VEZ - CONFIGURE AQUI ======================
  // Mude os valores abaixo para a data/hora ATUAL
  // Formato: segundo, minuto, hora, dia, dia_da_semana, mês, ano
  //
  // Exemplo atual (02 de Março de 2026, 22:48:00 - Segunda-feira)
  setPCA8565Time(0, 48, 22,     // segundos, minutos, horas
                 2, 1, 3, 26);  // dia, weekday (1=Segunda), mês, ano

  // Depois da primeira vez, comente ou remova a linha acima para não reconfigurar toda vez
}


void loop() {
  readPCA8565Time();  // Lê e imprime a hora
  delay(1000);        // Atualiza a cada 1 segundo
}