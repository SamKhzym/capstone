#include <SoftwareSerial.h>
#include "speedometer.h"
#include "comm_layer.h"
//#include "uart_comm_layer.h"
//#include "schemas.h"

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
#define W2_PWM_PIN 12 // Pin connected to w2 sensor
#define T1_PWM_PIN 10 // Pin connected to t1 sensor - NEED TO CHECK IF THIS WORKS
#define T2_PWM_PIN 2 // Pin connected to t2 sensor - NEED TO CHECK IF THIS WORKS

#define W1_LOWER_THRESHOLD_PCT 45
#define W1_UPPER_THRESHOLD_PCT 90

#define W2_LOWER_THRESHOLD_PCT 10
#define W2_UPPER_THRESHOLD_PCT 50

#define T1_LOWER_THRESHOLD_PCT 10
#define T1_UPPER_THRESHOLD_PCT 50

#define T2_LOWER_THRESHOLD_PCT 10
#define T2_UPPER_THRESHOLD_PCT 50

#define MIN_SPEED_MPS 0.05

#define CAR_WHEEL_RAD_M 0.05

#define DYNO_WHEEL_RAD_M 0.0635

#define SAMPLE_TIME_MS 5

unsigned long elapsedTime_ms = 0;

Speedometer *w1, *w2, *t1, *t2;

int speedCommand = 0;
int prevSpeedCommand = 0;

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
    pinMode(W2_PWM_PIN, INPUT); //waiting for validation on these three
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
    Serial.begin(115200);

    // Initialize motors to be stopped
    stopMotors();

    // setup speedometers
    w1 = new Speedometer("W1", W1_PWM_PIN, W1_LOWER_THRESHOLD_PCT, W1_UPPER_THRESHOLD_PCT, CAR_WHEEL_RAD_M, 5.0, MIN_SPEED_MPS);
    w2 = new Speedometer("W2", W2_PWM_PIN, W2_LOWER_THRESHOLD_PCT, W2_UPPER_THRESHOLD_PCT, CAR_WHEEL_RAD_M, 5.0, MIN_SPEED_MPS);
    // t1 = new Speedometer("T1", T1_PWM_PIN, T1_LOWER_THRESHOLD_PCT, T1_UPPER_THRESHOLD_PCT,CAR_WHEEL_RAD_M,  4.0, MIN_SPEED_MPS);
    // t2 = new Speedometer("T2", T2_PWM_PIN, T2_LOWER_THRESHOLD_PCT, T2_UPPER_THRESHOLD_PCT, CAR_WHEEL_RAD_M, 4.0, MIN_SPEED_MPS);

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

float generateSineCommand(float currTime, float minVal, float maxVal, float freq) {
    float amplitude = (maxVal - minVal) / 2;
    float bias = (maxVal + minVal) / 2;
    return amplitude * sinf(2 * M_PI * freq * currTime) + bias;
}

float generateSweptSineCommand(float currTime, float minVal, float maxVal, float freq, float freqRise) {
    if (currTime > 60) { return 0; }
    float amplitude = (maxVal - minVal) / 2;
    float bias = (maxVal + minVal) / 2;
    return amplitude * sinf(2 * M_PI * freq * freqRise * currTime * currTime) + bias;
}

void loop() {
    
    // loop around until enough time has passed (sample time from last execution)
    while (elapsedTime_ms > 0 && (millis() - elapsedTime_ms) < SAMPLE_TIME_MS) { }
    elapsedTime_ms = millis();

    if (Serial.available() > 0) {
        // Read the speed value from the UART
        prevSpeedCommand = speedCommand;
        speedCommand = Serial.parseInt();
        //Serial.println(speed);
    }
    else {
      speedCommand = prevSpeedCommand;
    }
    
    // Check if the speed value is valid
    if (speedCommand >= 100 && speedCommand <= 255) {
        // Example: Move forward with the received speed
        forward(speedCommand);
    } else {
        forward(0);
        //Serial.println("Invalid speed value received.");
    }

    // SpeedCommandPayload speedCommand = rxSpeedCommandPayload();
    // int speed = speedCommand.speedCommand_pwm; // uncomment when receiving from mcu

    //int speedCommand = (int)generateSweptSineCommand(millisToSec(elapsedTime_ms), 100, 255, 0.25, 0.15);

    // get vehicle speed from w1 speedometer
    float wheel1Speed = w1->getSpeed(millisToSec(elapsedTime_ms));
    float wheel2Speed = w2->getSpeed(millisToSec(elapsedTime_ms));
    // float dyno1Speed = t1->getSpeed(millisToSec(elapsedTime_ms));
    // float dyno2Speed = t2->getSpeed(millisToSec(elapsedTime_ms));

    // average out wheel speeds
    // float vehicleSpeed = (wheel1Speed + wheel2Speed + dyno1Speed + dyno2Speed) / 4;
    float vehicleSpeed = (wheel1Speed + wheel2Speed) / 2;

    // construct vehicle speed payload struct, serialize, and transmit payload
    VehicleSpeedPayload vehicleSpeedPayload = {
        .vehicleSpeed_mps = vehicleSpeed
        //.speedFaultActive = 0,
        //.rc = 0,
        //.crc = 0
    };
    //txVehicleSpeedPayload(&vehicleSpeedPayload);
    Serial.print("Vehicle Speed: ");
    Serial.println(vehicleSpeed);

#if DEBUG
    Serial.print("Elapsed Time: ");
    Serial.println(elapsedTime_ms);

    Serial.print("PWM_Speed: ");
    Serial.println(speedCommand);

    Serial.print("Wheel_Speed_W1: ");
    Serial.println(wheel1Speed);
    
    Serial.print("Wheel_Speed_W2: ");
    Serial.println(wheel2Speed);
    
    Serial.print("Millis: ");
    Serial.println(millis());
    Serial.println("=====================");
#endif

}