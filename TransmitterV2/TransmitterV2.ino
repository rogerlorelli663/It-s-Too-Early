//SendReceive.ino
#include<SPI.h>
#include <nRF24L01.h>
#include<RF24.h>
// CE, CSN pins
RF24 radio(7,8);
const byte addresses[6] = "00001";
byte buf[32];

// initial setup instructions
void setup(void){
  Serial.begin(9600);
  radio.begin();
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(115);
  radio.setDataRate(RF24_1MBPS);
  radio.openWritingPipe(addresses); // 00001
  radio.stopListening();
}

// receives threshold values through Serial connection, parses the String into 2 parts (lower and upper thresholds) and sends each part individually
void loop(void){
  String data = "default";
  String upper = "default";
  String lower = "default";
  char text[32] = "";
  if (Serial.available() > 0) {
    data = Serial.readStringUntil('\n');
    int index = data.indexOf(" ");
    upper = data.substring(0, index);
    lower = data.substring(index+1);
    Serial.print("I received: ");
    Serial.print(upper);
    Serial.print(" ");
    Serial.println(lower);
  }
  if(data.compareTo("default") != 0){
    upper.getBytes(buf, sizeof(upper));
    radio.write(&buf, sizeof(buf));
    lower.getBytes(buf, sizeof(lower));
    radio.write(&buf, sizeof(buf));
  }

}
