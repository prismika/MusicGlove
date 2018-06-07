//This sketch reads directly from the flex sensor and prints out the value for testing
//purposes. There might be another sketch that does this too but I can't find it.

//SWITCH USB TYPE TO SERIAL. MAKE SURE BOARD IS SET TO FLORA TEENSY
const int FLEXPIN = A11;

void setup() {
  pinMode(FLEXPIN, INPUT);
  Serial.begin(9600);
}

void loop() {
  Serial.println(analogRead(FLEXPIN));
  delay(50);

}
