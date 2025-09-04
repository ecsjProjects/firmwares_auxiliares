// Wire Master Writer
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Writes data to an I2C/TWI slave device
// Refer to the "Wire Slave Receiver" example for use with this

// Created 29 March 2006

// This example code is in the public domain.


#include <Wire.h>

void setup() {
  Wire.begin(); // join I2C bus (address optional for master)
   Serial.begin(9600);  // start serial for output
}

byte x = 0;

void loop() {

  Wire.beginTransmission(2); // endereço do escravo (ex: MPU6050)
  Wire.write(0x75);             // registrador a ser lido
  Wire.endTransmission();

  Wire.requestFrom(2, 1);    // pede 1 byte
  if (Wire.available()) {
    byte data = Wire.read();
    Serial.println(data, HEX);
  }

  delay(50);




  // Wire.beginTransmission(1); // transmit to device #8
  // // Wire.write("x is ");        // sends five bytes
  // // Wire.write(x);              // sends one byte
  // Wire.write(0);              // sends one byte
  // // Wire.write(1);              // sends one byte
  // // Wire.write(1);              // sends one byte
  // // Wire.write(0);              // sends one byte
  // // Wire.write(0);              // sends one byte
  // Wire.endTransmission();    // stop transmitting

  // x++;
  // // delay(500);
  // delay(50);


  // Wire.requestFrom(8, 6);    // request 6 bytes from slave device #8

  // while (Wire.available()) { // slave may send less than requested
  //   char c = Wire.read(); // receive a byte as character
  //   Serial.print(c);         // print the character
  // }

  // delay(500);

}
