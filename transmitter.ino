#include <RH_ASK.h>
#include <SPI.h>  // Needed for RH_ASK

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <DHT.h>

// ----- DHT11 -----
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// ----- BUTTON -----
#define BUTTON_PIN 3
bool lastButtonState = HIGH;

// ----- TX -----
#define TX_PIN 12
RH_ASK rf_driver(4000, 255, TX_PIN, -1);

// ----- OLED -----
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {

  // Serial monitor
  Serial.begin(9600);

  // Button
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // OLED
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Starting...");
  display.display();

  // DHT11
  dht.begin();

  // RF Transmitter
  rf_driver.init();

  delay(500);
}

void loop() {

  // ----- READ DHT11 -----
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // ----- SERIAL PRINT -----
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print("%    Temp: ");
  Serial.print(t);
  Serial.println("C");

  // ----- UPDATE OLED -----
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 0);
  display.print("H:");
  display.print(h);
  display.print("%");

  display.setCursor(0, 32);
  display.print("T:");
  display.print(t);
  display.print("C");

  display.display();

  // ----- BUTTON -----
  bool buttonState = digitalRead(BUTTON_PIN);

  if (buttonState != lastButtonState) {
    if (buttonState == LOW) {
      sendMessage("Group170Press");
    } else {
      sendMessage("Group170Release");
    }
    delay(50);  // debounce
  }

  lastButtonState = buttonState;

  delay(250);
}

void sendMessage(const char *msg) {
  rf_driver.send((uint8_t *)msg, strlen(msg));
  rf_driver.waitPacketSent();
}
