// Motor A (connected to AO1 and AO2)

//Motor A is Right-Hand Drive
const int AIN1 = 9; // AIN1/PH
const int AIN2 = 11; // AIN2/EN
const int PWMA = 6; // PWM control for motor A

// Motor B (connected to BO1 and BO2)

//Motor B is Left-Side-Drive
const int BIN1 = 7; // BIN1/PH
const int BIN2 = 8; // BIN2/EN
const int PWMB = 5; // PWM control for motor B
int increment_speed = 60;

void setup() {
    // Set all the motor control pins to outputs
    pinMode(AIN1, OUTPUT);
    pinMode(AIN2, OUTPUT);
    pinMode(PWMA, OUTPUT);
    
    pinMode(BIN1, OUTPUT);
    pinMode(BIN2, OUTPUT);
    pinMode(PWMB, OUTPUT);

    // Initialize motors to be stopped
    stopMotors();
}

void loop() {
    // Move forward
    forward(increment_speed);  // Full speed ahead
    delay(3000);   // Move for 3 seconds
    
    // Stop the motors
    stopMotors();
    delay(2000);   // Pause for 2 seconds

    // Move backward
    backward(increment_speed); // Move backward at a lower speed
    delay(3000);   // Move for 3 seconds

    // Stop the motors
    stopMotors();
    delay(2000);

    increment_speed += 10;
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
    digitalWrite(AIN1, HIGH);    // Motor A backward
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
