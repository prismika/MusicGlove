//For gyro communication:
#include <MPU6050.h>
#include <I2Cdev.h>
#include <Wire.h>

//Converts gyro reading from axis of choice (in this case Y) from (int16_t) into (int) in a MIDI-able form (0-127)
int getRotation(){
  int ret = (int)gyro.getRotationY()/256;
  return ret;
}

//Old MIDImessage that works with Hairless
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


//New MIDImessage that sends from Flora/teeny directly once set up as a midi device
//https://learn.adafruit.com/midi-drum-glove/code#adding-midi-support-to-flora
void MIDImessage(String command, byte data1, byte data2) {
  //1, text command.
  //2, param number or note value. C5 is 60.
  //3, param value or note velocity.
  if (command == "param") {
    usbMIDI.sendControlChange(data1, data2, 1)
  } else if (command == "stop") {
    usbMIDI.sendNoteOff(data1, data2, 1);
  } else if (command == "start") {
    usbMIDI.sendNoteOn(data1, data2, 1);
  } else {
    return;
  }
}



//------------------------------------------EXTRAS------------------------------------------

//||||||||||||||||||||||Transmitting||||||||||||||||||||||
These functions allow you to transmit all of the standard MIDI messages:
  usbMIDI.sendNoteOn(note, velocity, channel)
  usbMIDI.sendNoteOff(note, velocity, channel)
  usbMIDI.sendPolyPressure(note, pressure, channel)
  usbMIDI.sendControlChange(control, value, channel)
  usbMIDI.sendProgramChange(program, channel)
  usbMIDI.sendAfterTouch(pressure, channel)
  usbMIDI.sendPitchBend(value, channel)
  usbMIDI.sendSysEx(length, array)
  
MIDI messages are grouped together into USB packets. Up to 16 messages can transmit at once! They are
held for a brief time, not more than 1 ms, to facilitate grouping. You can use the send_now() function
to immediately allow any buffered messagse to transmit

  usbMIDI.send_now()

//||||||||||||||||||||||Receiving||||||||||||||||||||||
The easiest way to receive MIDI messages is with functions that are automatically called when each type of message is received. Usually you would create one or more of these:
  void OnNoteOn(byte channel, byte note, byte velocity)
  void OnNoteOff(byte channel, byte note, byte velocity)
  void OnVelocityChange(byte channel, byte note, byte velocity)
  void OnControlChange(byte channel, byte control, byte value)
  void OnProgramChange(byte channel, byte program)
  void OnAfterTouch(byte channel, byte pressure)
  void OnPitchChange(byte channel, int pitch)
For each function you create, you must use the corresponding "setHandle" functions to tell usbMIDI to call it when that message type is read.

  usbMIDI.setHandleNoteOff(OnNoteOff)
  usbMIDI.setHandleNoteOn(OnNoteOn)
  usbMIDI.setHandleVelocityChange(OnVelocityChange)
  usbMIDI.setHandleControlChange(OnControlChange)
  usbMIDI.setHandleProgramChange(OnProgramChange)
  usbMIDI.setHandleAfterTouch(OnAfterTouch)
  usbMIDI.setHandlePitchChange(OnPitchChange)
Once everything is set up, you then need the "read" function to actually read data. The callback functions are only called when you use read. There are 2 choices for read:

  usbMIDI.read();         // All Channels
If you give a specific channel number to read, messages to other channels are ignored, which is easier than having to check the channel number in your callback function.

  usbMIDI.read(channel);  // One Specific Channel (1 to 16)




