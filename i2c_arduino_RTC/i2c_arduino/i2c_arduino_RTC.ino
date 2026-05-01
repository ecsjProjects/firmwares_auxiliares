#include <Wire.h>
#include <stdint.h>

#define PCA8565_ADDR 0x51

// Function prototypes
byte bin2bcd(byte val);
byte bcd2bin(byte val);
void setTime(byte sec, byte min, byte hour, byte day, byte weekday, byte month, byte year);
void readTime();

void setup() {
  Wire.begin();       // Initialize I2C as master
  Serial.begin(9600); // Initialize serial for output
  
  // Example: Set time to 2024-03-05 22:14:00 (adjust as needed)
  // Note: Year is 00-99, weekday 0-6 (0=Sunday), month 1-12
  setTime(0, 32, 22, 5, 3, 3, 26); // sec, min, hour, day, weekday, month, year (24 for 2024)
}

void loop() {
  readTime();
  delay(1000); // Read every second
}

byte bin2bcd(byte val) {
  return ((val / 10) << 4) | (val % 10);
}

byte bcd2bin(byte val) {
  return ((val >> 4) * 10) + (val & 0x0F);
}

void setTime(byte sec, byte min, byte hour, byte day, byte weekday, byte month, byte year) {
  // Stop the clock
  Wire.beginTransmission(PCA8565_ADDR);
  Wire.write(0x00); // Control/Status 1 register
  Wire.write(0x20); // Set STOP bit (bit 5 = 1)
  Wire.endTransmission();

  // Set the time registers
  Wire.beginTransmission(PCA8565_ADDR);
  Wire.write(0x02); // Start from VL_seconds register
  Wire.write(bin2bcd(sec) & 0x7F); // Seconds (VL bit = 0)
  Wire.write(bin2bcd(min));
  Wire.write(bin2bcd(hour));
  Wire.write(bin2bcd(day));
  Wire.write(bin2bcd(weekday));
  Wire.write(bin2bcd(month)); // Century bit assumed 0 (2000-2099)
  Wire.write(bin2bcd(year));
  Wire.endTransmission();

  // Restart the clock
  Wire.beginTransmission(PCA8565_ADDR);
  Wire.write(0x00); // Control/Status 1 register
  Wire.write(0x00); // Clear STOP bit
  Wire.endTransmission();
}

void readTime() {
  // Request time registers
  Wire.beginTransmission(PCA8565_ADDR);
  Wire.write(0x02); // Start from VL_seconds
  Wire.endTransmission();

  Wire.requestFrom((uint8_t)PCA8565_ADDR, (uint8_t)(byte)7);
  if (Wire.available() == 7) {
    byte sec = bcd2bin(Wire.read() & 0x7F);
    byte min = bcd2bin(Wire.read() & 0x7F);
    byte hour = bcd2bin(Wire.read() & 0x3F);
    byte day = bcd2bin(Wire.read() & 0x3F);
    byte weekday = bcd2bin(Wire.read() & 0x07);
    byte month_century = Wire.read();
    byte month = bcd2bin(month_century & 0x1F);
    byte year = bcd2bin(Wire.read());

    // Determine full year based on century bit
    int full_year = (month_century & 0x80) ? 1900 + year : 2000 + year;

    // Print to serial
    Serial.print(full_year);
    Serial.print("-");
    if (month < 10) Serial.print("0");
    Serial.print(month);
    Serial.print("-");
    if (day < 10) Serial.print("0");
    Serial.print(day);
    Serial.print(" ");
    if (hour < 10) Serial.print("0");
    Serial.print(hour);
    Serial.print(":");
    if (min < 10) Serial.print("0");
    Serial.print(min);
    Serial.print(":");
    if (sec < 10) Serial.print("0");
    Serial.print(sec);
    Serial.print(" Weekday: ");
    Serial.println(weekday);
  } else {
    Serial.println("Error reading from PCA8565");
  }
}