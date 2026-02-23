#include <Wire.h>
#include <Adafruit_AS5600.h>

Adafruit_AS5600 as5600;

// ----- Variables for RPM -----
unsigned long lastTime = 0;
float lastAngleDeg = 0;
float rpm = 0;

void setup() {
  Serial.begin(115200);

  while (!Serial && millis() < 3000) {
    delay(10);
  }

  Wire.begin();

  if (!as5600.begin()) {
    while (1) {
      delay(10);
    }
  }

  as5600.enableWatchdog(false);
  as5600.setPowerMode(AS5600_POWER_MODE_NOM);
  as5600.setHysteresis(AS5600_HYSTERESIS_OFF);
  as5600.setOutputStage(AS5600_OUTPUT_STAGE_ANALOG_FULL);
  as5600.setSlowFilter(AS5600_SLOW_FILTER_16X);
  as5600.setFastFilterThresh(AS5600_FAST_FILTER_THRESH_SLOW_ONLY);

  as5600.setZPosition(0);
  as5600.setMPosition(4095);
  as5600.setMaxAngle(4095);

  lastTime = millis();
}

void loop() {

  if (!as5600.isMagnetDetected()) {
    return;
  }

  // ---- Get Raw Angle (0–4095) ----
  uint16_t rawAngle = as5600.getRawAngle();

  // ---- Convert to Degrees ----
  float angleDeg = (rawAngle * 360.0) / 4096.0;

  // ---- Calculate RPM ----
  unsigned long currentTime = millis();
  float deltaTime = (currentTime - lastTime) / 1000.0;  // seconds

  if (deltaTime >= 0.05) {  // update every 50ms

    float deltaAngle = angleDeg - lastAngleDeg;

    // Handle rollover at 0/360
    if (deltaAngle > 180)  deltaAngle -= 360;
    if (deltaAngle < -180) deltaAngle += 360;

    float revolutions = deltaAngle / 360.0;
    rpm = (revolutions / deltaTime) * 60.0;

    lastAngleDeg = angleDeg;
    lastTime = currentTime;
  }

  // ---- Labeled Output for Serial Plotter ----
  Serial.print("Angle(deg):");
  Serial.print(angleDeg);
  Serial.print(" ");
  Serial.print("RPM:");
  Serial.println(rpm);

  delay(10);
}
