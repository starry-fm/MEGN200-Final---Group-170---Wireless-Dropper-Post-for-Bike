#include <RH_ASK.h>
#include <Adafruit_SoftServo.h>
#include <SPI.h> // safe to include

// ----- SERVO SETTINGS -----
Adafruit_SoftServo mg996r;       // renamed to match MG996R
const int SERVO_PIN = 3;        
const int SERVO_ANGLE = 45;
const int HOME_ANGLE = 0;

// ----- TIMEOUT SETTINGS -----
const unsigned long MAX_HOLD_TIME = 6000; 
unsigned long servoStartTime = 0;
bool servoActive = false;

// ----- RF RECEIVER SETTINGS -----
const int RX_PIN = 12; 
RH_ASK rf_driver(4000, RX_PIN, 255, -1, false);

void setup() {
  mg996r.attach(SERVO_PIN);
  mg996r.write(HOME_ANGLE);

  if (!rf_driver.init()) while (1); // stop if RF fails

  Serial.begin(9600);
  Serial.println("Receiver started (Nano + MG996R + SoftServo)");
}

void loop() {
  mg996r.refresh(); // update SoftServo PWM

  uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
  uint8_t buflen = sizeof(buf);

  if (rf_driver.recv(buf, &buflen)) {
    String msg = "";
    for (int i = 0; i < buflen; i++) msg += (char)buf[i];

    Serial.print("Received: ");
    Serial.println(msg);

    if (msg == "Group170Press") {
      mg996r.write(SERVO_ANGLE);
      servoStartTime = millis();
      servoActive = true;
    } 
    else if (msg == "Group170Release") {
      mg996r.write(HOME_ANGLE);
      servoActive = false;
    }
  }

  if (servoActive && (millis() - servoStartTime > MAX_HOLD_TIME)) {
    mg996r.write(HOME_ANGLE);
    servoActive = false;
    Serial.println("Servo timeout, returning home");
  }
}
