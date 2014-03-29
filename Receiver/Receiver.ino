#include <SoftwareSerial.h>

SoftwareSerial midiSerial(2, 3); // RX, TX

byte instrument = -1;
byte note = -1;
byte volume = -1;
int duration = 50;

char buffer[10];
int length = 0;

void setup() {
  Serial.begin(57600);
  midiSerial.begin(31250);
  resetInput();
}

void loop() {
  char c;
  
  talkMIDI(0xB0, 0x07, 127);  // set main volume
  talkMIDI(0xB0, 0x00, 0x00); // use basic instruments
  
  while (Serial.available() > 0) {
    c = char( Serial.read() );
    
    if (c >= '0' && c <= '9') {
      buffer[length++] = c;
    }
    else if (c == ':') {
      instrument = getBufferValue();
    }
    else if (c == '@') {
      note = getBufferValue();
    }
    else if (c == '\n') {
      volume = getBufferValue();
      
      if (validateInput())
      {
        //printInput();
        playNote(instrument, note, volume);
      }
      else {
        //Serial.println("(Invalid input)");
      }
      
      resetInput();
    }
  }
  
  delay(duration / 2);
}

void resetInput() {
  instrument = -1;
  note = -1;
  volume = -1;
}

boolean validateInput() {
  if (instrument < 0 || instrument > 128) return false;
  if (note < 0 || note > 87) return false;
  return (volume >= 0 && volume <= 127);
}

void printInput(byte volume) {
  String s = String(instrument);
  s += ":" + String(note) + "@" + String(volume);
  Serial.println(s);
}

byte getBufferValue() {
  int v;
  
  buffer[length++] = '\0';
  v = atoi(buffer);
  memset(buffer, '\0', sizeof(buffer));
  length = 0;
  
  return v;
}

void playNote(byte instrument, byte note, byte volume) {
  talkMIDI(0xC0, instrument, 127);
  noteOn(0, note, volume);
  delay(duration);
  noteOff(0, note, volume);
  delay(duration / 2);
}


//Send a MIDI note-on message.  Like pressing a piano key
//channel ranges from 0-15
void noteOn(byte channel, byte note, byte attack_velocity) {
  talkMIDI( (0x90 | channel), note, attack_velocity);
}

//Send a MIDI note-off message.  Like releasing a piano key
void noteOff(byte channel, byte note, byte release_velocity) {
  talkMIDI( (0x80 | channel), note, release_velocity);
}

//Plays a MIDI note. Doesn't check to see that cmd is greater than 127, or that data values are less than 127
void talkMIDI(byte cmd, byte data1, byte data2) {
  midiSerial.write(cmd);
  midiSerial.write(data1);

  //Some commands only have one data byte. All cmds less than 0xBn have 2 data bytes 
  //(sort of: http://253.ccarh.org/handout/midiprotocol/)
  if( (cmd & 0xF0) <= 0xB0)
    midiSerial.write(data2);
}

