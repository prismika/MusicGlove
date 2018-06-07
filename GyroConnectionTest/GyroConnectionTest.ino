#include <MPU6050.h>
#include <I2Cdev.h>
#include <Wire.h>

MPU6050 gyro;
const int LEDPIN = 13;
void setup() {
  Wire.begin();
  gyro.initialize();
  pinMode(LEDPIN, OUTPUT);

  if (!gyro.testConnection()) {
    while (true) {
      digitalWrite(LEDPIN, HIGH);
      delay(500);
      digitalWrite(LEDPIN, LOW);
      delay(500);
    }
  }
}

void loop() {
  delay(300);
}
