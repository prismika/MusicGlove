// USB MIDI receive example, Note on/off -> LED on/off
// contributed by Alessandro Fasan

int ledPin = 7;

void OnNoteOn(byte channel, byte note, byte velocity)
{
  digitalWrite(ledPin, HIGH);
}

void OnNoteOff(byte channel, byte note, byte velocity)
{
  digitalWrite(ledPin, LOW);
}

void setup()
{
  pinMode(ledPin, OUTPUT);
  usbMIDI.setHandleNoteOff(OnNoteOff);
  usbMIDI.setHandleNoteOn(OnNoteOn) ;
  digitalWrite(ledPin, HIGH);
  delay(1000);
  digitalWrite(ledPin, LOW);
  delay(1000);
}

void loop()
{
  usbMIDI.read();
  usbMIDI.sendNoteOn(61, 99, 1);
  delay(100);
  usbMIDI.sendNoteOff(61, 99, 1);
}
