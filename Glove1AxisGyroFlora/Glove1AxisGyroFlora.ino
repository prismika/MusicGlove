#include <MPU6050.h>
#include <I2Cdev.h>
#include <Wire.h>
#include <Glove.h>

//MPU6050 Flora Teensy
//  Vcc   VBatt 3.3V
//  GND   GND   GND
//  SCL   D3    19
//  SDA   D2    18
//  ADO   GND   (A)GND
//  INT   D0(?) 0(?)

//To enter debug mode:
//1) Set LOG to 1 or 2 depending on verbosity needed.
//2) Comment out the else if block from MIDIMessage
//3) Switch board type from MIDI to Serial above in tools
const int LOG = 0;
const int GLOVE_VERSION = 2;  //1: Original Flora
                              //2: Mk II with teensy
const int READ_TYPE = 0;

//---------------------------------CONSTANTS TO TWEAK------------------------------
const int MESSAGES_PER_SECOND = 100;
const uint8_t GYRO_RANGE = 3; //From 0 (+-250 degrees/sec) to 3 (+-2000 degrees/sec)
const int BUFFERSIZE = 15; //Used for smoothing values by averaging
const int CURRENT_OVERVALUE = 12;  //When averaging, how much EXTRA weight should be given to the most recent reading?
const int THRESHOLD = 2; //Threshold for noise gate

//--------------------------------------Setup---------------------------------------
MPU6050 gyro;
Glove glove(GLOVE_VERSION, BUFFERSIZE, MESSAGES_PER_SECOND, CURRENT_OVERVALUE, THRESHOLD);

void setup() {
  if (LOG > 0) Serial.begin(9600);
  lg("Serial open", 1);

  glove.debugMode(LOG);

  glove.initialize(gyro, GYRO_RANGE);
  glove.setReadType(0);
  MIDImessage("param", 0, 0);
}

//---------------------------------------Loop-----------------------------------------------------------
int prevMessage = 0;
void loop() {
  glove.read();
  int smoothedRot = glove.getSmoothReading();

  if (smoothedRot != prevMessage) {
    MIDImessage("param", 0, smoothedRot);
    glove.setAnalogLight(smoothedRot);
    prevMessage = smoothedRot;
  }
  delay(1000 / MESSAGES_PER_SECOND);
}

//------------------------------------Helper functions----------------------------------
void MIDImessage(String command, byte data1, byte data2) {
  //1, text command.
  //2, param number or note value. C5 is 60.
  //3, param value or note velocity.
  if (LOG >= 1) {
    Serial.println("MIDI: " + command + " " + data1 + " " + data2);
    return;
  }
//ENTERING DEBUG MODE? COMMENT FROM HERE:

  if (command == "param") {
    usbMIDI.sendControlChange(data1, data2, 1);
  } else if (command == "stop") {
    usbMIDI.sendNoteOff(data1, data2, 1);
  } else if (command == "start") {
    usbMIDI.sendNoteOn(data1, data2, 1);
  } else {
    return;
  }

//TO HERE
}

void lg(char msg[], int verbosity){
  if(verbosity <= LOG)Serial.println(msg);
}




