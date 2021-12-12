#include <IRremote.h> // include the IRremote library
#include<RF24.h>
#define IR_RECEIVE_PIN      2
#define DECODE_NEC  
#define P1A 10 // define pin 10as for P1A
#define P2A 9 // define pin 11 as for P2A
#define EN12 11 // define pin 9 as for 1,2EN enable
#define MAXSTEPS 10


RF24 radio(40,53);
const byte addresses[6] = "00001";

String st1 = "";         // incoming serial byte
String defaultRead = "";
int insideSensorValue = 0;  // variable to store the value coming from the sensor
int outsideSensorValue = 0;  // variable to store the value coming from the sensor
int upperThreshold = 900;
int lowerThreshold = 900;
bool closed = false;
int IRpin = 2;
int insideLeftPin = A0;
int outsideLeftPin = A1;
int insideRightPin = A14;
int outsideRightPin = A15;
int state;
int window;
int steps;

typedef enum{
    OPEN,
    CLOSED
} windowStates;

typedef enum{
    AUTO,
    REMOTE
} states;

// Engages DC motor in pulses of 50 milliseconds until either inside sensor value is greater than the upper threshold or the maximum number of steps has been reached, this is to ensure that the curtains are not over drawn in 1 direction.
void autoCloseCurtain()
{
  while((insideSensorValue > upperThreshold && steps < MAXSTEPS) || steps < MAXSTEPS){
    digitalWrite(P1A,HIGH);
    digitalWrite(P2A,LOW);
    delay(50);
    digitalWrite(P1A,LOW);
    digitalWrite(P2A,LOW);
    readSensor();
    steps++;
  }
  steps = 0;
  window = CLOSED;
}

// Engages DC motor in pulses of 50 milliseconds until either outside sensor value is lower than the lower threshold or the maximum number of steps has been reached, this is to ensure that the curtains are not over drawn in 1 direction.
void autoOpenCurtain()
{
  while((outsideSensorValue < lowerThreshold && steps < MAXSTEPS) || steps < MAXSTEPS){
    digitalWrite(P2A,HIGH);
    digitalWrite(P1A,LOW);
    delay(50);
    digitalWrite(P1A,LOW);
    digitalWrite(P2A,LOW);
    readSensor();
    steps++;
  } 
  steps = 0;
  window = OPEN;
}

// Debugging print
void printThresholds(bool received)
{
  if(received)
  {
    Serial.println("----------Received-----------");
  }
  Serial.print("Upper Threshold: ");
  Serial.println(upperThreshold);
  Serial.print("Lower Threshold: ");
  Serial.println(lowerThreshold);
  if(received)
  {
    Serial.println("-----------------------------");
  }
}

// read from 4 sensors, 2 out and 2 in. Take the average of each paired set and save.
void readSensor()
{
    int insideLeftSensorValue = analogRead(insideLeftPin);
    int outsideLeftSensorValue = analogRead(outsideLeftPin);
    int insideRightSensorValue = analogRead(insideRightPin);
    int outsideRightSensorValue = analogRead(outsideRightPin);
    outsideSensorValue = (outsideLeftSensorValue + outsideRightSensorValue) / 2;
    insideSensorValue = (insideLeftSensorValue + insideRightSensorValue) / 2;
}

// initial setup instructions
void setup() {
    // start serial port at 9600 bps:
    Serial.begin(9600);
    radio.begin();
    radio.setPALevel(RF24_PA_MAX);
    radio.setChannel(115);
    radio.setDataRate(RF24_1MBPS);
    radio.openReadingPipe(0, addresses);
    while (!Serial) {
        ; // wait for serial port to connect. Needed for native USB port only
    }
    defaultRead = Serial.readString();
    //attachInterrupt(digitalPinToInterrupt(RECEIVER_PIN), IRDecode, CHANGE);
    state = AUTO;
    window = OPEN;
    steps = 0;
    IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK, USE_DEFAULT_FEEDBACK_LED_PIN);

    Serial.print(F("Ready to receive IR signals at pin "));
    Serial.println(IR_RECEIVE_PIN);
    pinMode(P1A, OUTPUT);// define pin as OUTPUT for P1A
    pinMode(P2A, OUTPUT);// define pin as OUTPUT for P2A
    pinMode(EN12, OUTPUT);// define pin as OUTPUT for 1,2EN
}


// Checks for updates from webserver
// Checks for IR signal, decodes if received and either changes action state or window state depending on command.
// Read from sensors, 
// if outside and inside value is greater than upper threshold close curtains.
// if outside sensor value is less than lower threshold open curtains.
void loop() {
  radio.startListening();
  delay(500);
  if(radio.available()){
      char text[32] = "";
      radio.read(&text, sizeof(text));
      String str = String(text);
      upperThreshold = str.toInt();
      radio.read(&text, sizeof(text));
      str = String(text);
      lowerThreshold = str.toInt();
      printThresholds(true);
    }
  if (IrReceiver.decode()) {
    IrReceiver.printIRResultShort(&Serial);
    IrReceiver.resume(); // Enable receiving of the next value

    if (IrReceiver.decodedIRData.command == 0x45 && state == AUTO) {
      // Set to Remote control
      Serial.println("Remote Control");
      state = REMOTE;
    } else if (IrReceiver.decodedIRData.command == 0x45 && state == REMOTE) {
      // Set to Automatic control
      Serial.println("Automatic Control");
      state = AUTO;
    } else if (IrReceiver.decodedIRData.command == 0x43 && state == REMOTE && window == CLOSED) {
      // Open Curtains
      Serial.println("Opening");
      autoOpenCurtain();
      Serial.println("---------Opened---------");
    } else if (IrReceiver.decodedIRData.command == 0x44 && state == REMOTE && window == OPEN) {
      // Close Curtains
      Serial.println("Closing");
      autoCloseCurtain();
      Serial.println("---------Closed---------");
    }
  }

  if(state == AUTO)
  {
    
    readSensor();
    printThresholds(false);
    Serial.print("Outside: ");
    Serial.println(outsideSensorValue);
    Serial.print("Inside: ");
    Serial.println(insideSensorValue);
    if(window == CLOSED && outsideSensorValue < lowerThreshold)
    {
      Serial.println("Opening");
      autoOpenCurtain();
      Serial.println("---------Opened---------");
    }
    else if(window == OPEN && outsideSensorValue >= upperThreshold && insideSensorValue >= upperThreshold)
    {
      Serial.println("Closing");
      autoCloseCurtain();
      Serial.println("---------Closed---------");
    }
    delay(4000);
  }
}