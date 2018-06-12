#include "Arduino.h"
#include "Glove.h"
#include "Adafruit_NeoPixel.h"


Glove::Glove(int gloveVersionIn, int bufferSizeIn, int messagesPerSecondIn, int currentOvervalueIn, int thresholdIn){
	bufferSize = bufferSizeIn;
	messagesPerSecond = messagesPerSecondIn;
	currentOvervalue = currentOvervalueIn;
	threshold = thresholdIn;
	gloveVersion = gloveVersionIn;
	readType = 0;
	
	switch(gloveVersionIn){
		case 1: ledPin = 7; break;
		case 2: ledPin = 13; break;
	}
	
	bufferPosition = 0;
}

void Glove::initialize(MPU6050 gyroIn, int gyroRangeIn){
	
	gyro = gyroIn;
	Wire.begin();
	gyro.initialize();
	if(!gyro.testConnection()){
		bool on = false;
		while(true){
			digitalWrite(ledPin, on ? LOW : HIGH);
			on = !on;
			delay(500);
		}
	}
	gyro.setFullScaleGyroRange(gyroRangeIn);
	
	for(int i = 0; i < bufferSize; i++){
		readBuffer[i] = getRotation();
		delay(1000/messagesPerSecond);
	}
	
	pinMode(ledPin, OUTPUT);
	
	if(gloveVersion == 1){
		int PIXEL_BRIGHTNESS = 55;
		pixel = Adafruit_NeoPixel(1, 8, NEO_GRB + NEO_KHZ800);
    	pixel.begin();
    	pixel.show();
		colorPixel(0,   0,                PIXEL_BRIGHTNESS, 250);
		colorPixel(0,   PIXEL_BRIGHTNESS, 0,                250);
		colorPixel(0,   0,                0,                0);
	}
}

void Glove::read(){
	int newRead = -1;
	switch(readType){
		case 0: newRead = getRotation();
			if(abs(newRead) < threshold) newRead = 0;
			break;
		case 1: newRead = getPosition();
			break;
	}
	bufferPosition = (bufferPosition + 1) % bufferSize;
	readBuffer[bufferPosition] = newRead;
}

int Glove::getSmoothReading(){
	if(debugLevel >= 2){
		Serial.print("[ ");
		for(int i=0; i<bufferSize; i++){
			if(i == bufferPosition){
				Serial.print("|");
				Serial.print(readBuffer[i]);
				Serial.print("| ");
			}else{
				Serial.print(readBuffer[i]);
				Serial.print(" ");
			}
		}
		Serial.println("]");
	}
	//Average the buffer
	int ret = 0;
	for(int i = 0; i < bufferSize; i++){
		ret += readBuffer[i];
	}
	if(debugLevel >= 2) Serial.print(ret);
	ret += readBuffer[bufferPosition] * currentOvervalue;
	ret /= bufferSize + currentOvervalue;
	//Return non-negative number
	return abs(ret);
}

void Glove::setReadType(int type){
	readType = type;
}

int Glove::getRotation(){
	return gyro.getRotationY()/256;
}

int Glove:: getPosition(){
	int ret = gyro.getAccelerationZ()/256;
	
}

void Glove::setAnalogLight(int level){
	switch(gloveVersion){
	case 1:
		colorPixel(0,0,level, 0);
	}
}

void Glove::debugMode(int level){
	debugLevel = level;
}

void Glove::colorPixel(int red, int green, int blue, int delayTime) {
  //If delayTime = 0, delay is skipped);
  if(gloveVersion != 1) return;
  pixel.setPixelColor(0, red, green, blue);
  pixel.show();
  if (delayTime > 0) {
    delay(delayTime);
  }
}

void Glove::lg(char msg[]){
	if(debugLevel > 0){
		Serial.print(msg);
	}
}

