#include "WifiPort2.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DHT.h"

//WiFi / DataPacket
struct DataPacket {
  int AnalogCheck;     
  bool ButtonPressed; 
} data;

WifiPort<DataPacket> WifiSerial;

// Pins
const int buttonPin = 3;
const int DHTPin = 2;
#define DHTTYPE DHT11

//DHT Sensor
DHT dht(DHTPin, DHTTYPE);

//OLED Display
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Variables
bool lastButtonState = false;
bool signalSent = false;

void setup() {
  Serial.begin(115200);

  // Initialize button
  pinMode(buttonPin, INPUT_PULLUP);

  // Initialize DHT
  dht.begin();

  // Initialize OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed")); //Keep in Flash to conserve RAM
    while (1);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Initialize WiFi as transmitter
  WifiSerial.begin("group170dropper", "dropperYeah", WifiPortType::Transmitter);
  Serial.println("WiFi transmitter ready");
}

void loop() {
  data.AnalogCheck = millis();

  // Button handling
  bool buttonState = !digitalRead(buttonPin); // pressed = HIGH, AI cleaned up code here
  if (buttonState != lastButtonState) {
    data.ButtonPressed = buttonState;
    lastButtonState = buttonState;
    Serial.print("ButtonPressed: "); Serial.println(data.ButtonPressed);
  }

  // Send data over WifiPort2
  signalSent = WifiSerial.sendData(data);
  if (!signalSent) {
    Serial.println("WiFi Send Problem");
  }

  // Read DHT11
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  // Update OLED
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Temp: ");
  if (isnan(temp)) display.print("Err"); else display.print(temp);
  display.println(" C");

  display.print("Humidity: ");
  if (isnan(hum)) display.print("Err"); else display.print(hum);
  display.println(" %");

  // Button / Signal Indicator
  display.setCursor(0, 50);
  display.print("Button: ");
  display.println(data.ButtonPressed ? "ON" : "OFF"); //AI cleaned up code

  display.print("Signal: ");
  display.println(signalSent ? "OK" : "FAIL"); //AI cleaned up code

  display.display();

  delay(200);
}

//OLED Display Credit: https://www.instructables.com/Monochrome-096-i2c-OLED-display-with-arduino-SSD13/
//DHT11 Credit: https://docs.oyoclass.com/unoeditor/Libraries/dht/
