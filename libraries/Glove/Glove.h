#include "Arduino.h"
#include "MPU6050.h"

//TODO Add ifdef things to this

//TODO Facade for boards
//TODO 	Led pins
//TODO	Pixel

//TODO Move MPU6050 things to inside of library

//TODO sendMidi function

class Glove{
	public:
		Glove(int gloveVersion, int bufferSize, int messagesPerSecond, int currentOvervalue, int threshold);
		void initialize(MPU6050 gyroIn, int gyroRange);
		void read();
		int getSmoothRotation();
		int getRotation();
	private:
		MPU6050 gyro;
		int gloveVersion;
		int ledPin;
		int readBuffer[15];
		int bufferSize;
		int bufferPosition;
		int messagesPerSecond;
		int currentOvervalue;
		int threshold;
};

