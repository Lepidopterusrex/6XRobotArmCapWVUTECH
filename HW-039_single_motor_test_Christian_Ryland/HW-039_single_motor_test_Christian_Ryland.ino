/*
  Arduino Uno + BTS7960 Single Motor Test
  ----------------------------------------
  Serial Monitor input:
    Enter number between -255 and 255

    Positive = Forward
    Negative = Reverse
    0        = Stop
*/

// ===== PIN DEFINITIONS (MATCHING YOUR WORKING TEST) =====
const int RPWM = 3;   // R_PWM
const int LPWM = 5;   // L_PWM
const int REN  = 2;   // R_EN
const int LEN  = 4;   // L_EN

void setup() {
  Serial.begin(115200);

  pinMode(RPWM, OUTPUT);
  pinMode(LPWM, OUTPUT);
  pinMode(REN, OUTPUT);
  pinMode(LEN, OUTPUT);

  // Enable H-bridge
  digitalWrite(REN, HIGH);
  digitalWrite(LEN, HIGH);

  stopMotor();

  Serial.println("BTS7960 Serial Motor Control Ready");
  Serial.println("Enter speed from -255 to 255:");
}

void loop() {
  if (Serial.available()) {

    int speed = Serial.parseInt();   // Read signed integer
    speed = constrain(speed, -255, 255);

    setMotorSpeed(speed);

    Serial.print("Motor speed set to: ");
    Serial.println(speed);

    while (Serial.available()) Serial.read();  // Clear buffer
  }
}

void setMotorSpeed(int speed) {

  // Ensure driver stays enabled
  digitalWrite(REN, HIGH);
  digitalWrite(LEN, HIGH);

  if (speed > 0) {
    // Forward
    analogWrite(LPWM, 0);
    analogWrite(RPWM, speed);
  }
  else if (speed < 0) {
    // Reverse
    analogWrite(RPWM, 0);
    analogWrite(LPWM, -speed);
  }
  else {
    stopMotor();
  }
}

void stopMotor() {
  analogWrite(RPWM, 0);
  analogWrite(LPWM, 0);
}