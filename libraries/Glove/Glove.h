#include "Arduino.h"
#include "MPU6050.h"
#include "Adafruit_NeoPixel.h"

//TODO Add ifdef things to this

//Logs

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
		int getSmoothReading();
		void setReadType(int type);
		//0: Y-Axis rotation
		//1: Y-Axis position
		int getRotation();
		int getPosition();
		void setAnalogLight(int level);
		
		void debugMode(int level);
	private:
		MPU6050 gyro;
		int gloveVersion;
		
		void lg(char msg[]);
		int debugLevel;
		
		Adafruit_NeoPixel pixel;
		void colorPixel(int red, int green, int blue, int delayTime);
		int readType;
		int ledPin;
		int readBuffer[15];
		int bufferSize;
		int bufferPosition;
		int messagesPerSecond;
		int currentOvervalue;
		int threshold;
};

