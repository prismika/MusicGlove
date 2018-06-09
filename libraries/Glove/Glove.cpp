#include "Arduino.h"
#include "Glove.h"


Glove::Glove(int gloveVersionIn, int bufferSizeIn, int messagesPerSecondIn, int currentOvervalueIn, int thresholdIn){
	bufferSize = bufferSizeIn;
	messagesPerSecond = messagesPerSecondIn;
	currentOvervalue = currentOvervalueIn;
	threshold = thresholdIn;
	gloveVersion = gloveVersionIn;
	
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
}

void Glove::read(){
	int newRead = getRotation();
	if(abs(newRead) < threshold) newRead = 0;
	bufferPosition = (bufferPosition + 1) % bufferSize;
	readBuffer[bufferPosition] = newRead;
}

int Glove::getSmoothRotation(){
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

int Glove::getRotation(){
	return gyro.getRotationY()/256;
}

void Glove::debugMode(int level){
	debugLevel = level;
}

void Glove::lg(char msg[]){
	if(debugLevel > 0){
		Serial.print(msg);
	}
}

