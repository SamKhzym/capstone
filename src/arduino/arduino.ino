#include <SoftwareSerial.h>
#include "speedometer.h"

// Motor A (connected to AO1 and AO2)
// Motor A is Right-Hand Drive
#define AIN1 9 // AIN1/PH
#define AIN2 11 // AIN2/EN
#define PWMA 6 // PWM control for motor A

// Motor B (connected to BO1 and BO2)
// Motor B is Left-Side-Drive
#define BIN1 7 // BIN1/PH
#define BIN2 8 // BIN2/EN
#define PWMB 5 // PWM control for motor B

#define W1_PWM_PIN 4 // Pin connected to w1 sensor
#define W2_PWM_PIN 2 // Pin connected to w2 sensor
#define T1_PWM_PIN 7 // Pin connected to t1 sensor
#define T2_PWM_PIN 8 // Pin connected to t2 sensor

#define THRESHOLD_DUTY_CYCLE_PCT 0.7
#define MIN_SPEED_MPS 0.05
#define CAR_WHEEL_RAD_M 0.05

#define SAMPLE_TIME_MS 5

unsigned long elapsedTime_ms = 0;

Speedometer* w1; /*, w2, t1, t2*/

typedef union
{
  float number;
  uint8_t bytes[4];
} FLOATUNION_t;

void initPins() {

    pinMode(AIN1, OUTPUT);
    pinMode(AIN2, OUTPUT);
    pinMode(PWMA, OUTPUT);

    pinMode(BIN1, OUTPUT);
    pinMode(BIN2, OUTPUT);
    pinMode(PWMB, OUTPUT);

}

void setup() {

    // set all pwm pins to inputs
    pinMode(W1_PWM_PIN, INPUT);
    // pinMode(W2_PWM_PIN, INPUT); //waiting for validation on these three
    // pinMode(T1_PWM_PIN, INPUT);
    // pinMode(T2_PWM_PIN, INPUT);

    // Set all the motor control pins to outputs
    pinMode(AIN1, OUTPUT);
    pinMode(AIN2, OUTPUT);
    pinMode(PWMA, OUTPUT);

    pinMode(BIN1, OUTPUT);
    pinMode(BIN2, OUTPUT);
    pinMode(PWMB, OUTPUT);

    // Start the hardware serial port for communication with MCU
    Serial.begin(9600);

    // Initialize motors to be stopped
    stopMotors();

    // setup speedometers
    w1 = new Speedometer(W1_PWM_PIN, THRESHOLD_DUTY_CYCLE_PCT, CAR_WHEEL_RAD_M, MIN_SPEED_MPS);
    // w2 = new Speedometer(W2_PWM_PIN, THRESHOLD_DUTY_CYCLE_PCT, CAR_WHEEL_RAD_M, MIN_SPEED_MPS);
    // t1 = new Speedometer(T1_PWM_PIN, THRESHOLD_DUTY_CYCLE_PCT, CAR_WHEEL_RAD_M, MIN_SPEED_MPS);
    // t2 = new Speedometer(T2_PWM_PIN, THRESHOLD_DUTY_CYCLE_PCT, CAR_WHEEL_RAD_M, MIN_SPEED_MPS);

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
}

// Function to move backward
void backward(int speed) {
    // Set the motor direction for both sides
    digitalWrite(AIN1, HIGH);   // Motor A backward
    digitalWrite(AIN2, LOW);   
    digitalWrite(BIN1, LOW);    // Motor B backward
    digitalWrite(BIN2, HIGH);   

    // Set the motor speed for both sides
    analogWrite(PWMA, speed);   // Motor A speed
    analogWrite(PWMB, speed);   // Motor B speed
}

// Function to stop motors
void stopMotors() {
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, LOW);
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, LOW);
}

float millisToSec(unsigned long milliseconds) {
    return (float)milliseconds / 1000.0;
}

void sendFloatUart(float value) {

    FLOATUNION_t valueUnion = value;
    for (int i = 0; i < sizeof(float); i++) {
        Serial.print(valueUnion.bytes[i]);
    }

}

void loop() {

    // loop around until enough time has passed (sample time from last execution)
    while (elapsedTime_ms > 0 && (millis() - elapsedTime_ms) < SAMPLE_TIME_MS) {}
    elapsedTime_ms += SAMPLE_TIME_MS;

    // check for new speed command, send command to motors
    if (Serial.available() > 0) {

        // Read the speed value from the Microcontroller UART
        int speed = Serial.parseInt();

        // Check if the speed value is valid
        if (speed >= 0 && speed <= 255) {
            forward(speed);
        }

    }

    float vehicleSpeed = w1->getSpeed(millisToSec(elapsedTime_ms));
    sendFloatUart(vehicleSpeed);

}