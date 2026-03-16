#include "DFRobot_DF2301Q.h"

// --- Voice Module Setup ---
// On Teensy 4.1, we use Hardware Serial1
DFRobot_DF2301Q_UART DF2301Q(&Serial1);

// --- Motor Structure and Pins ---
struct Motor {
  char name;
  const char* label;
  uint8_t rpwmPin;
  uint8_t lpwmPin;
};

const int ENABLE_PIN = 8;

// Motor indices: 0=Base, 1=Shoulder, 2=Elbow, 3=WristPitch, 4=WristRoll
Motor motors[] = {
  {'B', "Base",       2, 3},
  {'S', "Shoulder",   4, 5},
  {'E', "Elbow",      6, 7},
  {'P', "WristPitch", 9, 10},
  {'R', "WristRoll",  11, 12}
};

const int NUM_MOTORS = sizeof(motors) / sizeof(motors[0]);

// ---------------- SETUP ----------------

void setup() {
  Serial.begin(115200);
  
  // Initialize Voice Module
  // Teensy 4.1 Serial1 uses pins 0(RX) and 1(TX) by default
  while (!(DF2301Q.begin())) {
    Serial.println("Voice Module connection failed!");
    delay(3000);
  }
  Serial.println("Voice Module Begin OK!");

  // Motor Pin Setup
  pinMode(ENABLE_PIN, OUTPUT);
  digitalWrite(ENABLE_PIN, HIGH);

  for (int i = 0; i < NUM_MOTORS; i++) {
    pinMode(motors[i].rpwmPin, OUTPUT);
    pinMode(motors[i].lpwmPin, OUTPUT);
    analogWrite(motors[i].rpwmPin, 0);
    analogWrite(motors[i].lpwmPin, 0);
  }

  Serial.println("Robot Arm Ready. Commands: B S E P R <speed>, X, or Voice.");
}

// ---------------- LOOP ----------------

void loop() {
  // 1. Check for Serial Commands (Manual Overide)
  if (Serial.available()) {
    String line = Serial.readStringUntil('\n');
    line.trim();
    if (line.length() > 0) {
      processCommand(line);
    }
  }

  // 2. Check for Voice Commands
  uint8_t CMDID = DF2301Q.getCMDID();
  if (CMDID != 0) {
    Serial.print("Voice CMDID Detected: ");
    Serial.println(CMDID); // For debugging
    handleVoiceCommand(CMDID);
  }

  delay(50); // Small delay for stability
}

// ---------------- VOICE COMMAND HANDLER ----------------

void handleVoiceCommand(uint8_t id) {
  switch (id) {
    case 5: // "Custom Command 1"
      Serial.println("Voice Action: Elbow Positive");
      setMotorSpeed(motors[2].rpwmPin, motors[2].lpwmPin, 40); // Speed 200
      break;

    case 6: // "Custom Command 2"
      Serial.println("Voice Action: Elbow Negative");
      setMotorSpeed(motors[2].rpwmPin, motors[2].lpwmPin, -40); // Speed -200
      break;

    case 7: // "Custom Command 3"
      Serial.println("Voice Action: Elbow Stop");
      setMotorSpeed(motors[2].rpwmPin, motors[2].lpwmPin, 0);
      break;

    default:
      Serial.print("Voice ID ");
      Serial.print(id);
      Serial.println(" has no assigned action.");
      break;
  }
}

// ---------------- MOTOR CONTROL ----------------

void setMotorSpeed(uint8_t rpwm, uint8_t lpwm, int speed) {
  speed = constrain(speed, -255, 255);

  if (speed > 0) {
    analogWrite(lpwm, 0);
    analogWrite(rpwm, speed);
  } 
  else if (speed < 0) {
    analogWrite(rpwm, 0);
    analogWrite(lpwm, -speed);
  } 
  else {
    analogWrite(rpwm, 0);
    analogWrite(lpwm, 0);
  }
}

// ---------------- STOP ALL ----------------

void stopAll() {
  for (int i = 0; i < NUM_MOTORS; i++) {
    setMotorSpeed(motors[i].rpwmPin, motors[i].lpwmPin, 0);
  }
}

// ---------------- COMMAND PARSER ----------------

void processCommand(String line) {
  char cmd = toupper(line.charAt(0));

  if (cmd == 'X') {
    stopAll();
    Serial.println("All motors stopped");
    return;
  }

  String value = line.substring(1);
  value.trim();
  int speed = value.toInt();

  for (int i = 0; i < NUM_MOTORS; i++) {
    if (motors[i].name == cmd) {
      setMotorSpeed(motors[i].rpwmPin, motors[i].lpwmPin, speed);
      Serial.print(motors[i].label);
      Serial.print(" speed ");
      Serial.println(speed);
      return;
    }
  }
  Serial.println("Invalid serial command");
}