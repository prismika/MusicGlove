#include <MPU6050.h>
#include <I2Cdev.h>
#include <Wire.h>

#include <Adafruit_NeoPixel.h>

//Vcc - VBatt
//GND - GND
//SCL - D3
//SDA - D2
//ADO - GND
//INT - D0 (?)

const int LEDPIN = 7;
const int BUTTONPIN = 10;
const int MPU_addr=0x68;  // I2C address of the MPU-6050
                          // 0x68 if AD0 is HIGH
                          // 0X69 if AD0 is LOW
                          
//---------------------------------CONSTANTS TO TWEAK------------------------------
const int MESSAGES_PER_SECOND = 100;
const uint8_t GYRO_RANGE = 2; //From 0 (+-250 degrees/sec) to 3 (+-2000 degrees/sec)
const int BUFFERSIZE = 15; //Used for smoothing values by averaging
const int CURRENT_OVERVALUE = 12;  //When averaging, how much EXTRA weight should be given to the most recent reading?
const int ZERO_OVERVALUE = 0;     //When averaging, how much EXTRA EXTRA weight should be given to readings of zero on top of CURRENT_OVERVALUE?
const int THRESHOLD = 2; //Threshold for noise gate

const int PIXEL_BRIGHTNESS = 55;
//--------------------------------------Setup---------------------------------------

int rotBuffer[BUFFERSIZE];
int bufferPos = 0;
int prevMessage = 0;
MPU6050 gyro;
Adafruit_NeoPixel pixel = Adafruit_NeoPixel(1, 8, NEO_GRB + NEO_KHZ800);

void setup(){
  pinMode(LEDPIN, OUTPUT);
  pinMode(BUTTONPIN, INPUT_PULLUP);
  pixel.begin();
  pixel.show();
  Wire.begin();
  gyro.initialize();
  if(!gyro.testConnection()){
    bool on = false;
    while(true){
      digitalWrite(LEDPIN, on ? LOW : HIGH);
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
  
  colorPixel(0,   0,                PIXEL_BRIGHTNESS, 250);
  colorPixel(0,   PIXEL_BRIGHTNESS, 0,                250);
  colorPixel(0,0,0,0);
}

//---------------------------------------Loop-----------------------------------------------------------
void loop(){
  if(digitalRead(BUTTONPIN) == LOW){
    linkSequence();
  }
  
  
  int rot = abs(getRotation());
  if(rot < THRESHOLD){
    rot = 0;
  }
  rotBuffer[bufferPos] = rot;
  bufferPos = (bufferPos+1)%BUFFERSIZE;
  
  //Calculate smoothedRot by averaging buffer values.
  int smoothedRot;
  int zeroWeight = 0;
  if(rot == 0){
    zeroWeight = ZERO_OVERVALUE;
  }
  for(int i = 0; i < BUFFERSIZE; i++){
    smoothedRot += rotBuffer[i];
  }
  smoothedRot += (CURRENT_OVERVALUE + zeroWeight) * rot;
  smoothedRot /= BUFFERSIZE+CURRENT_OVERVALUE+zeroWeight;

  if(smoothedRot != prevMessage){
    MIDImessage("param", 0, smoothedRot);
    colorPixel(0,0,smoothedRot,0);
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
  if (command == "param") {
    usbMIDI.sendControlChange(data1, data2, 1);
  } else if (command == "stop") {
    usbMIDI.sendNoteOff(data1, data2, 1);
  } else if (command == "start") {
    usbMIDI.sendNoteOn(data1, data2, 1);
  } else {
    return;
  }

}

void colorPixel(int red, int green, int blue, int delayTime){
  //If delayTime = 0, delay is skipped);
  pixel.setPixelColor(0, red,green,blue);
  pixel.show();
  if(delayTime > 0){
    delay(delayTime);
  }
}

void linkSequence(){
  colorPixel(0,PIXEL_BRIGHTNESS,0,200);
  MIDImessage("param",0,0);
  colorPixel(0,0,0,0);
  return;
}




