#include <RH_ASK.h>
#include <Adafruit_SoftServo.h>
#include <SPI.h> // safe to include

// ----- SERVO SETTINGS -----
Adafruit_SoftServo mg995;
const int SERVO_PIN = 3;        // Any digital pin works
const int SERVO_ANGLE = 45;
const int HOME_ANGLE = 0;

// ----- TIMEOUT SETTINGS -----
const unsigned long MAX_HOLD_TIME = 6000; // 6 seconds
unsigned long servoStartTime = 0;
bool servoActive = false;

// ----- RF RECEIVER SETTINGS -----
const int RX_PIN = 12; 
RH_ASK rf_driver(4000, RX_PIN, 255, -1, false); // 4000 bps, RX pin, disable interrupts

void setup() {
  // Servo setup
  mg995.attach(SERVO_PIN);
  mg995.write(HOME_ANGLE);

  // RF setup
  if (!rf_driver.init()) {
    while (1); // stop if RF fails
  }

  // Serial monitor for debugging
  Serial.begin(9600);
  Serial.println("Receiver started (Nano + Adafruit SoftServo)");
}

void loop() {
  // Update SoftServo PWM signals
  mg995.refresh();

  // ----- RECEIVE MESSAGE -----
  uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
  uint8_t buflen = sizeof(buf);

  if (rf_driver.recv(buf, &buflen)) {
    // Convert received bytes to String
    String msg = "";
    for (int i = 0; i < buflen; i++) {
      msg += (char)buf[i];
    }

    // Debug print
    Serial.print("Received: ");
    Serial.println(msg);

    // ----- SERVO CONTROL -----
    if (msg == "Group170Press") {
      mg995.write(SERVO_ANGLE);
      servoStartTime = millis();
      servoActive = true;
    }
    else if (msg == "Group170Release") {
      mg995.write(HOME_ANGLE);
      servoActive = false;
    }
  }

  // ----- SERVO TIMEOUT -----
  if (servoActive && (millis() - servoStartTime > MAX_HOLD_TIME)) {
    mg995.write(HOME_ANGLE);
    servoActive = false;
    Serial.println("Servo timeout, returning home");
  }
}
