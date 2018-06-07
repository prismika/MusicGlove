#include "Arduino.h"
#include "Glove.h"


Glove::Glove(int gloveVersionIn, int bufferSizeIn, int messagesPerSecondIn, int currentOvervalueIn, int thresholdIn){
	ledPin = 7;
	bufferSize = bufferSizeIn;
	messagesPerSecond = messagesPerSecondIn;
	currentOvervalue = currentOvervalueIn;
	threshold = thresholdIn;
	
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
	//Average the buffer
	int ret = 0;
	for(int i = 0; i < bufferSize; i++){
		ret += readBuffer[bufferPosition];
	}
	ret += readBuffer[bufferPosition] * currentOvervalue;
	ret /= bufferSize + currentOvervalue;
	return abs(ret);
}

int Glove::getRotation(){
	return gyro.getRotationY()/256;
}

