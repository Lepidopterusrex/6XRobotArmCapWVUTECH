/*
  Teensy 4.1 + 5 BTS7960 Motor Drivers
  Shared Enable Pin

  Motors:
    B = Base
    S = Shoulder
    E = Elbow
    P = Wrist Pitch
    R = Wrist Roll

  Serial Commands:
    B <speed>
    S <speed>
    E <speed>
    P <speed>
    R <speed>
    X (stop all)
*/

struct Motor {
  char name;
  const char* label;
  uint8_t rpwmPin;
  uint8_t lpwmPin;
};

const int ENABLE_PIN = 8;

Motor motors[] = {

  {'B', "Base",       2, 3},
  {'S', "Shoulder",   4, 5},
  {'E', "Elbow",      6, 7},
  {'P', "WristPitch", 9,10},
  {'R', "WristRoll", 11,12}

};

const int NUM_MOTORS = sizeof(motors) / sizeof(motors[0]);

// ---------------- SETUP ----------------

void setup() {

  Serial.begin(115200);

  pinMode(ENABLE_PIN, OUTPUT);
  digitalWrite(ENABLE_PIN, HIGH);

  for(int i=0;i<NUM_MOTORS;i++){

    pinMode(motors[i].rpwmPin,OUTPUT);
    pinMode(motors[i].lpwmPin,OUTPUT);

    analogWrite(motors[i].rpwmPin,0);
    analogWrite(motors[i].lpwmPin,0);
  }

  Serial.println("Robot Arm Ready");
  Serial.println("Commands: B S E P R <speed> or X");
}

// ---------------- LOOP ----------------

void loop() {

  if(Serial.available()){

    String line = Serial.readStringUntil('\n');
    line.trim();

    if(line.length()>0){
      processCommand(line);
    }

  }

}

// ---------------- MOTOR CONTROL ----------------

void setMotorSpeed(uint8_t rpwm, uint8_t lpwm, int speed){

  speed = constrain(speed,-255,255);

  if(speed > 0){

    analogWrite(lpwm,0);
    analogWrite(rpwm,speed);

  }
  else if(speed < 0){

    analogWrite(rpwm,0);
    analogWrite(lpwm,-speed);

  }
  else{

    analogWrite(rpwm,0);
    analogWrite(lpwm,0);

  }

}

// ---------------- STOP ALL ----------------

void stopAll(){

  for(int i=0;i<NUM_MOTORS;i++){

    analogWrite(motors[i].rpwmPin,0);
    analogWrite(motors[i].lpwmPin,0);

  }

}

// ---------------- COMMAND PARSER ----------------

void processCommand(String line){

  char cmd = toupper(line.charAt(0));

  if(cmd=='X'){

    stopAll();
    Serial.println("All motors stopped");
    return;
  }

  String value = line.substring(1);
  value.trim();
  int speed = value.toInt();

  for(int i=0;i<NUM_MOTORS;i++){

    if(motors[i].name == cmd){

      setMotorSpeed(motors[i].rpwmPin,motors[i].lpwmPin,speed);

      Serial.print(motors[i].label);
      Serial.print(" speed ");
      Serial.println(speed);

      return;
    }

  }

  Serial.println("Invalid command");

}
