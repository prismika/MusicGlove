#include <MPU6050.h>
#include <I2Cdev.h>
#include <Wire.h>


//Vcc - 5V (Possibly 3.3V)
//GND - GND
//SCL - A5
//SDA - A4
//ADO - GND
//INT - D2


const int MPU_addr=0x68;  // I2C address of the MPU-6050
                          // 0x68 if AD0 is HIGH
                          // 0X69 if AD0 is LOW

const int MESSAGES_PER_SECOND = 10;
const uint8_t GYRO_RANGE = 3; //From 0 (+-250 degrees/sec) to 3 (+-2000 degrees/sec)
const int BUFFERSIZE = 4; //Used for smoothing values by averaging
const int THRESHOLD = BUFFERSIZE; //Threshold for noise gate

int16_t rotBuffer[BUFFERSIZE];
int bufferPos;
MPU6050 gyro;

void setup(){
  Wire.begin();
  Serial.begin(9600);
  Serial.print("Initializing gyro... ");
  gyro.initialize();
  Serial.println("Successful");
  Serial.print("Testing gyro connections... ");
  Serial.println(gyro.testConnection() ? "Connection test successful" : "Connection test failed. That's fine. Probably.");
  gyro.setFullScaleGyroRange(GYRO_RANGE);

  Serial.println("Filling buffer...");
  for(int i = 0; i < BUFFERSIZE; i++){
    rotBuffer[i] = getRotation();
    delay(1000/MESSAGES_PER_SECOND);
  }
  bufferPos = 0;
}
void loop(){
  int rot = getRotation();
  if(abs(rot) < THRESHOLD){
    rot = 0;
  }
  rotBuffer[bufferPos] = rot;
  bufferPos = (bufferPos+1)%BUFFERSIZE;
  
  //Calculate smoothedRot by averaging buffer values.
  int smoothedRot;
  for(int i = 0; i < BUFFERSIZE; i++){
    smoothedRot += rotBuffer[i];
  }
  smoothedRot /= BUFFERSIZE;
  
//  for(int i = 0; i < abs(smoothedRot); i++){
//    Serial.print("|");
//  }
  Serial.println(smoothedRot);
  delay(1000/MESSAGES_PER_SECOND);
}

int getRotation(){
  int ret = (int)gyro.getRotationY()/256;
  return ret;
}










