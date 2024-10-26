#include <SoftwareSerial.h>
 uint8_t incoming_msg;
 // Define the pins for SoftwareSerial
//const int rxPin = 10; // RX pin
//const int txPin = 11; // TX pin
 //SoftwareSerial mySerial(rxPin, txPin);

void setup() {
  // Start the hardware serial port for debugging
    Serial.begin(9600);
    // Start the software serial port
    //mySerial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available() > 0) {
    incoming_msg = Serial.read(); 
    Serial.println(incoming_msg);
    //Serial.println("%d", incoming_msg);

  }
//Serial.println("I am living");
}
