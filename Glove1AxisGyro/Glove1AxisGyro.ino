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
                          
//---------------------------------CONSTANTS TO TWEAK------------------------------
const int MESSAGES_PER_SECOND = 50;
const uint8_t GYRO_RANGE = 2; //From 0 (+-250 degrees/sec) to 3 (+-2000 degrees/sec)
const int BUFFERSIZE = 8; //Used for smoothing values by averaging
const int CURRENT_OVERVALUE = 3; //When averaging, how much EXTRA weight should be given to the most recent reading?
const int THRESHOLD = 1; //Threshold for noise gate

//--------------------------------------Setup---------------------------------------

int rotBuffer[BUFFERSIZE];
int bufferPos = 0;
int prevMessage = 0;
MPU6050 gyro;

void setup(){
  pinMode(13, OUTPUT);
  
  bool on = false;
  while(!Serial){
      digitalWrite(13, on ? HIGH : LOW);
      on = !on;
      delay(500);
  }
  Wire.begin();
  Serial.begin(9600);
  gyro.initialize();
  if(!gyro.testConnection()){
    while(true){
      digitalWrite(13, on ? HIGH : LOW);
      on = !on;
      delay(500);
    }
  }
  gyro.setFullScaleGyroRange(GYRO_RANGE);
  
  for(int i = 0; i < BUFFERSIZE; i++){
    rotBuffer[i] = getRotation();
    delay(1000/MESSAGES_PER_SECOND);
  }
  MIDImessage("param",0,0);
}

//---------------------------------------Loop-----------------------------------------------------------
void loop(){
  int rot = abs(getRotation());
  if(rot < THRESHOLD){
    rot = 0;
  }
  rotBuffer[bufferPos] = rot;
  bufferPos = (bufferPos+1)%BUFFERSIZE;
  
  //Calculate smoothedRot by averaging buffer values.
  int smoothedRot;
  for(int i = 0; i < BUFFERSIZE; i++){
    smoothedRot += rotBuffer[i];
  }
  smoothedRot += CURRENT_OVERVALUE * rot;
  smoothedRot /= BUFFERSIZE+CURRENT_OVERVALUE;

  if(smoothedRot != prevMessage){
    MIDImessage("param", 0, smoothedRot);
    prevMessage = smoothedRot;
  }
  delay(1000/MESSAGES_PER_SECOND);
}

//----------------------------------------Helper functions----------------------------------
int getRotation(){
  int ret = (int)gyro.getRotationY()/256;
  return ret;
}

void MIDImessage(String command, byte data1, byte data2) {
  //1, text command.
  //2, param number or note value. C5 is 60.
  //3, param value or note velocity.
  byte commandID;
  if (command == "param") {
    commandID = 176;
  } else if (command == "stop") {
    commandID = 128;
  } else if (command == "start") {
    commandID = 144;
  } else {
    return;
  }

  Serial.write(commandID);
  Serial.write(data1);
  Serial.write(data2);
}








