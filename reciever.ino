#include "WifiPort2.h"
#include <Servo.h>

// ----- Servo Setup -----
Servo mg996r;
const int SERVO_PIN = 3;
const int SERVO_MAX_ANGLE = 50; // Max angle
const int SERVO_HOME_ANGLE = 0; // Home position

// ----- Servo Timing -----
const unsigned long PRESS_TIMEOUT = 250;
unsigned long lastPressTime = 0;
bool pressed = false;

// ----- Wifi / DataPacket -----
struct DataPacket {
  int AnalogCheck;      // Debug / timestamp
  bool ButtonPressed;   // Only button in packet
} data;

WifiPort<DataPacket> WifiSerial;

void setup() {
  Serial.begin(115200);
  Serial.println("WiFi Servo Controller Starting");

  // Servo initialization
  mg996r.attach(SERVO_PIN);
  mg996r.write(SERVO_HOME_ANGLE);
  Serial.println("Servo at HOME");
  delay(500);

  // WiFi initialization as receiver
  WifiSerial.begin("group170dropper", "dropperYeah", WifiPortType::Receiver);
  Serial.println("WiFi receiver ready");
}

void loop() {
  // Keep connection alive
  WifiSerial.autoReconnect();

  // Update debug variable
  data.AnalogCheck = millis();

  // Check for incoming data
  if (WifiSerial.checkForData()) {
    data = WifiSerial.getData();

    // ----- Servo actuation -----
    if (data.ButtonPressed) {
      if (!pressed) {
        Serial.println(">>> PRESS");
        mg996r.write(SERVO_MAX_ANGLE); // Move to max angle
      }
      lastPressTime = millis();
      pressed = true;
    }

    // Auto-return to home
    if (pressed && (millis() - lastPressTime > PRESS_TIMEOUT)) {
      Serial.println(">>> HOME");
      mg996r.write(SERVO_HOME_ANGLE);
      pressed = false;
    }

    // ----- Debug output -----
    Serial.print("ButtonPressed: ");
    Serial.print(data.ButtonPressed);
    Serial.print(" | AnalogCheck: ");
    Serial.println(data.AnalogCheck);

    delay(10);
  }
}
