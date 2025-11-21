#include <RH_ASK.h>
#include <SPI.h>

const int BUTTON_PIN = 2;  // Change to your button pin
const int TX_PIN = 12;     // Change to your transmitter data pin

RH_ASK rf_driver(4000, -1, TX_PIN, -1, false);

const char MSG[] = "Group170Press";
const uint8_t MSG_LEN = 13;

bool lastButtonState = HIGH;  // Assuming button uses pull-up
unsigned long lastDebounceTime = 0;
const unsigned long DEBOUNCE_DELAY = 50;

void setup() {
  Serial.begin(9600);
  Serial.println("RF Transmitter Starting");
  
  // Initialize button with internal pull-up
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  // Initialize RF transmitter
  if (!rf_driver.init()) {
    Serial.println("RF init FAILED!");
  } else {
    Serial.println("RF transmitter ready");
  }
}

void loop() {
  int buttonState = digitalRead(BUTTON_PIN);
  
  // Debounce and detect button press (LOW = pressed with pull-up)
  if (buttonState == LOW && lastButtonState == HIGH) {
    if (millis() - lastDebounceTime > DEBOUNCE_DELAY) {
      
      Serial.println("Button pressed - sending message");
      
      // Send message multiple times for reliability
      for (int i = 0; i < 5; i++) {
        rf_driver.send((uint8_t*)MSG, MSG_LEN);
        rf_driver.waitPacketSent();
        delay(10);
      }
      
      Serial.println("Message sent");
      lastDebounceTime = millis();
    }
  }
  
  lastButtonState = buttonState;
  delay(10);  // Small delay for stability
}
