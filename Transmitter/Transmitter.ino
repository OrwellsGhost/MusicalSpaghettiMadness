int changeBarrier = 2;
int entryBarrier = 10;
int maximumValue = 85;
int calibration = 2;

int pinCount = 6;
int pins[] = {A0,A1,A2,A3,A4,A5};
int vals[] = {0,0,0,0,0,0};
byte instruments[] = {12, 9, 91, 104, 8, 118};

void setup() {
  Serial.begin(57600);
}

void loop() {
  String s;
  int val;
  
  for (int i = 0; i < pinCount; i++) {
    val = getAnalogValue(pins[i]);
    
    if (val == vals[i]) continue;
    
    if (val < entryBarrier) {
      vals[i] = val;
      continue;
    }
    
    if (abs(val - vals[i]) <= changeBarrier && val < maximumValue) {
      continue;
    }
    
    vals[i] = val;
    
    s = String(instruments[i]) + ":" + String(val) + "@127";
    Serial.println(s);
  }
  
  delay(50);
}

int getAnalogValue(int pin) {
  int val = analogRead(pin);
  
  //s = "I=" + String(i) + " AR=" + String(val);
  //Serial.println(s);
  
  val = int(float(val) * calibration);
  
  if (val > maximumValue) val = maximumValue;
  if (val < entryBarrier) val = 0;
  
  return val;
}

