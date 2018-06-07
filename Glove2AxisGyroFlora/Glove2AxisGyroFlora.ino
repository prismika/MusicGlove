//SWITCH BOARD TO FLORA TEENSY IN TOOLS
//SWITCH TYPE TO MIDI IN TOOLS

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
const int FLEXPIN = A11;
//Reading at resting state of sensor
int const FLEX_THRESHOLD = 920;


const int MPU_addr = 0x68; // I2C address of the MPU-6050
// 0x68 if AD0 is HIGH
// 0X69 if AD0 is LOW

//---------------------------------CONSTANTS TO TWEAK------------------------------
const int MESSAGES_PER_SECOND = 4800;
const uint8_t GYRO_RANGE = 3; //From 0 (+-250 degrees/sec) to 3 (+-2000 degrees/sec)
const int BUFFERSIZE = 32; //Used for smoothing values by averaging
const int CURRENT_OVERVALUE = BUFFERSIZE / 4; //When averaging, how much EXTRA weight should be given to the most recent reading?
const int ZERO_OVERVALUE = 0;     //When averaging, how much EXTRA EXTRA weight should be given to readings of zero on top of CURRENT_OVERVALUE?
const int THRESHOLD = 2; //Threshold for noise gate

const int AXISCOUNT = 1;
const int PIXEL_BRIGHTNESS = 30;
//--------------------------------------Setup---------------------------------------

int rotBuffer[AXISCOUNT][BUFFERSIZE];
int bufferPos[AXISCOUNT];
int prevMessage[AXISCOUNT];
MPU6050 gyro;
Adafruit_NeoPixel pixel = Adafruit_NeoPixel(1, 8, NEO_GRB + NEO_KHZ800);

void setup() {
  pinMode(LEDPIN, OUTPUT);
  pinMode(BUTTONPIN, INPUT_PULLUP);
  pinMode(FLEXPIN, INPUT);
  pixel.begin();
  pixel.show();
  
  Wire.begin();
  gyro.initialize();
  if (!gyro.testConnection()) {
    bool on = false;
    while (true) {
      digitalWrite(LEDPIN, on ? LOW : HIGH);
      on = !on;
      delay(500);
    }
  }
  gyro.setFullScaleGyroRange(GYRO_RANGE);
  {
    bool on = false;
    while (!Serial) {
      colorPixel(on ? PIXEL_BRIGHTNESS : 0, 0, 0, 0);
      on = !on;
      delay(500);
    }
  }
  for (int i = 0; i < AXISCOUNT; i++) {
    prevMessage[i] = 0;
    bufferPos[i] = 0;
    for (int j = 0; j < BUFFERSIZE; j++) {
      rotBuffer[i][j] = 0;
    }
  }
  MIDImessage("param", 0, 0);

  colorPixel(0,   0,                PIXEL_BRIGHTNESS, 250);
  colorPixel(0,   PIXEL_BRIGHTNESS, 0,                250);
  colorPixel(0, 0, 0, 0);
}

//---------------------------------------Loop-----------------------------------------------------------
void loop() {
  //||||||||||||||||||||||||||||||||||||||||||||||||||||TODO: CUT DOWN RUN TIME OF THIS LOOP||||||||||||||||||||||||||||||||||||||||||||||||||
  if (digitalRead(BUTTONPIN) == LOW) {
    linkSequence();
  }
  long rot;
  //Run readings and calculations for each axis.

  for (int axis = 0; axis < AXISCOUNT; axis++) {
    rot = getRotation(axis);
    rot = abs(rot);
    if (rot < THRESHOLD) {
      rot = 0;
    }
    rotBuffer[axis][bufferPos[axis]] = rot;
    bufferPos[axis] = (bufferPos[axis] + 1) % BUFFERSIZE;

    //Calculate smoothedRot by averaging buffer values.
    long smoothedRot = 0;
    //int zeroWeight = 0;
    //    //Add specified extra averaging weight to readings of zero.
    //    if (rot == 0) {
    //      zeroWeight = ZERO_OVERVALUE;
    //    }
    for (int i = 0; i < BUFFERSIZE; i++) {
      smoothedRot += rotBuffer[axis][i];
    }
    smoothedRot += CURRENT_OVERVALUE * rot;
    //smoothedRot += (CURRENT_OVERVALUE + zeroWeight) * rot;
    smoothedRot /= BUFFERSIZE + CURRENT_OVERVALUE;
    //smoothedRot /= BUFFERSIZE + CURRENT_OVERVALUE + zeroWeight;

    //smoothedRot = min(255, smoothedRot);

    if (smoothedRot != prevMessage[axis]) {
      //----------TODO FIX THE FLEX-------------
      //if(analogRead(FLEXPIN) < FLEX_THRESHOLD){
        MIDImessage("param", axis, smoothedRot);
      //}
      //colorPixelByAxis(axis, smoothedRot / 8);
      colorPixel(0, 0, smoothedRot / 5, 0);
      prevMessage[axis] = smoothedRot;
    }
  }
  //delay(1000 / MESSAGES_PER_SECOND);
}

//----------------------------------------Helper functions----------------------------------
int getRotation(int axis) {
  int ret;
  switch (axis) {
    case 0:
      ret = (int)(gyro.getRotationY() / 256);
      break;
    case 1:
      ret = (int)(gyro.getRotationX() / 256);
      break;
    case 2:
      ret = (int)(gyro.getRotationZ() / 256);
      break;
  }
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

void colorPixel(int red, int green, int blue, int delayTime) {
  //If delayTime = 0, delay is skipped);
  pixel.setPixelColor(0, red, green, blue);
  pixel.show();
  if (delayTime > 0) {
    delay(delayTime);
  }
}

void colorPixelByAxis(int axis, int brightness) {
  switch (axis) {
    case 0:
      colorPixel(0, 0, brightness, 0);
      break;
    case 1:
      colorPixel(0, brightness, 0, 0);
      break;
    case 2:
      colorPixel(brightness, 0, 0, 0);
      break;
  }
}

void linkSequence() {
  colorPixel(PIXEL_BRIGHTNESS / 3, PIXEL_BRIGHTNESS / 3, PIXEL_BRIGHTNESS / 3 , 0);
  while (digitalRead(BUTTONPIN) == LOW) {
    delay(5);
  }
  while (digitalRead(BUTTONPIN) == HIGH) {
    delay(5);
  }
  for (int axis = 0; axis < AXISCOUNT; axis++) {
    colorPixelByAxis(axis, PIXEL_BRIGHTNESS);
    MIDImessage("param", axis, 0);
    delay(200);
    colorPixel(0, 0, 0, 200);
  }
  return;
}




