// pin numbers for the bit banged SPI pins
#define bbSCK 5
#define bbMISO 4
#define bbMOSI 3
#define bbCS 2

// for checking the read value with analogRead
#define analogPin A0

#define usDelay 25

void setup() {
  // put your setup code here, to run once:
  pinMode(bbSCK, OUTPUT);
  pinMode(bbMISO, INPUT);
  pinMode(bbMOSI, OUTPUT);
  pinMode(bbCS, OUTPUT);

  Serial.begin(9600);
  while (!Serial) {
    ;
  }

  reset();
}

void loop() {
  // put your main code here, to run repeatedly:

  // read value using our function
  int res = bbReadValue();
  Serial.print("Bit banging value: ");
  Serial.println(res);

  // read value using built-in analog reading hardware
  int aRead = analogRead(analogPin);
  Serial.print("Analog read value: ");
  Serial.println(aRead);
  Serial.println("");

  //wait one second before reading again
  delay(1000); 
}

/*
 * Set all pin outputs to starting positions (disable chip)
 */
void reset() {
  digitalWrite(bbCS, HIGH); //chip select off
  digitalWrite(bbSCK, HIGH); //start high
  digitalWrite(bbMOSI, LOW); //high is start, so set low
}

/*
 * Read a value - using bit banging!
 */
int bbReadValue() {
  int value = 0; //store the read value here
  int readBit; //store the last read bit here
  
  // initiate communication
  digitalWrite(bbCS, LOW);
  
  digitalWrite(bbSCK, LOW); //rising edge - change Din
  digitalWrite(bbMOSI, HIGH); //start bit
  delayMicroseconds(usDelay);
  digitalWrite(bbSCK, HIGH); //rising edge - chip latches Din
  delayMicroseconds(usDelay);

  // single/diff bit
  digitalWrite(bbSCK, LOW); 
  digitalWrite(bbMOSI, HIGH); //single/diff bit, 1 = single
  delayMicroseconds(usDelay);
  digitalWrite(bbSCK, HIGH); 
  delayMicroseconds(usDelay);

  // address D2-D0 + sampling cycle
  for (int i = 3; i >= 0; i--) {
    digitalWrite(bbSCK, LOW); 
    digitalWrite(bbMOSI, LOW); //bit #2, #1, #0 of address
    delayMicroseconds(usDelay);
    digitalWrite(bbSCK, HIGH); 
    delayMicroseconds(usDelay);
  }

  // null bit + D9 through D0
  for (int i = 10; i >= 0; i--) {
    digitalWrite(bbSCK, LOW); 
    delayMicroseconds(usDelay);
    digitalWrite(bbSCK, HIGH); 
    readBit = digitalRead(bbMISO);
    value = value << 1;
    value += readBit;
    delayMicroseconds(usDelay);
  }

  reset();
  return value;
}
