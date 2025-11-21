#include <ServoTimer2.h>
#include <RH_ASK.h>
#include <SPI.h>

ServoTimer2 mg996r;

const int SERVO_PIN = 3;
const int RX_PIN = 12;

// ServoTimer2 uses microseconds (750-2250 range typical)
const int SERVO_ANGLE_US = 1500;  // ~45 degrees
const int HOME_ANGLE_US = 750;    // ~0 degrees

const unsigned long PRESS_TIMEOUT = 250;
unsigned long lastPressTime = 0;
bool pressed = false;

RH_ASK rf_driver(4000, RX_PIN, 255, -1, false);

const char VALID_MSG[] = "Group170Press";
const uint8_t VALID_LEN = 13;

void setup() {
  Serial.begin(9600);
  Serial.println("RF Servo Controller Starting");
  
  // Initialize servo
  mg996r.attach(SERVO_PIN);
  mg996r.write(HOME_ANGLE_US);
  Serial.println("Servo at HOME");
  delay(500);
  
  // Initialize RF receiver
  if (!rf_driver.init()) {
    Serial.println("RF init FAILED!");
  } else {
    Serial.println("RF receiver ready");
  }
}

void loop() {
  uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
  uint8_t buflen = sizeof(buf);

  if (rf_driver.recv(buf, &buflen)) {
    
    // ONLY act on EXACT match - ignore everything else
    if (buflen == VALID_LEN && memcmp(buf, VALID_MSG, VALID_LEN) == 0) {
      
      if (!pressed) {
        Serial.println(">>> PRESS");
        mg996r.write(SERVO_ANGLE_US);
      }
      
      lastPressTime = millis();
      pressed = true;
    }
  }

  // Auto-return to home
  if (pressed && (millis() - lastPressTime > PRESS_TIMEOUT)) {
    Serial.println(">>> HOME");
    mg996r.write(HOME_ANGLE_US);
    pressed = false;
  }
}
