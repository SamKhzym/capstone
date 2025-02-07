#include <SoftwareSerial.h>

// Motor A (connected to AO1 and AO2)
// Motor A is Right-Hand Drive
const int AIN1 = 9; // AIN1/PH
const int AIN2 = 11; // AIN2/EN
const int PWMA = 6; // PWM control for motor A

// Motor B (connected to BO1 and BO2)
// Motor B is Left-Side-Drive
const int BIN1 = 7; // BIN1/PH
const int BIN2 = 8; // BIN2/EN
const int PWMB = 5; // PWM control for motor B

// Define the pins for SoftwareSerial
const int rxPin = 10; // RX pin
const int txPin = 11; // TX pin

// Create a SoftwareSerial object


void setup() {
    // Set all the motor control pins to outputs
    pinMode(AIN1, OUTPUT);
    pinMode(AIN2, OUTPUT);
    pinMode(PWMA, OUTPUT);
    
    pinMode(BIN1, OUTPUT);
    pinMode(BIN2, OUTPUT);
    pinMode(PWMB, OUTPUT);

    

    // Start the hardware serial port for debugging
    Serial.begin(9600);
    
    // Initialize motors to be stopped
    stopMotors();
    Serial.println("Setup complete. Motors stopped.");
}

// Function to move forward
void forward(int speed) {
    // Set the motor direction for both sides
    digitalWrite(AIN1, LOW);   // Motor A forward
    digitalWrite(AIN2, HIGH);    
    digitalWrite(BIN1, HIGH);   // Motor B forward
    digitalWrite(BIN2, LOW);
    
    // Set the motor speed for both sides
    analogWrite(PWMA, speed);   // Motor A speed
    analogWrite(PWMB, speed);   // Motor B speed

    Serial.print("Moving forward at speed: ");
    Serial.println(speed);
}

// Function to move backward
void backward(int speed) {
    // Set the motor direction for both sides
    digitalWrite(AIN1, HIGH);    // Motor A backward
    digitalWrite(AIN2, LOW);   
    digitalWrite(BIN1, LOW);    // Motor B backward
    digitalWrite(BIN2, HIGH);   
    
    // Set the motor speed for both sides
    analogWrite(PWMA, speed);   // Motor A speed
    analogWrite(PWMB, speed);   // Motor B speed

    Serial.print("Moving backward at speed: ");
    Serial.println(speed);
}

// Function to stop motors
void stopMotors() {
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, LOW);
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, LOW);

    Serial.println("Motors stopped.");
}

void loop() {
    if (Serial.available() > 0) {
        // Read the speed value from the UART

        int speed = Serial.parseInt();
        
        // Check if the speed value is valid
        if (speed >= 0 && speed <= 255) {
            // Example: Move forward with the received speed
            forward(speed);
            delay(3000);   // Move for 3 seconds
            
            // Stop the motors
            stopMotors();
            delay(2000);   // Pause for 2 seconds

            // Example: Move backward with the received speed
            backward(speed);
            delay(3000);   // Move for 3 seconds

            // Stop the motors
            stopMotors();
            delay(2000);
        } else {
            Serial.println("Invalid speed value received.");
        }
    }
}
