#include <RH_ASK.h>
#include <SPI.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Pin definitions
const int BUTTON_PIN = 3;
const int DHT_PIN = 2;
const int TX_PIN = 12;

// DHT11 sensor
#define DHTTYPE DHT11
DHT dht(DHT_PIN, DHTTYPE);

// OLED display (128x64)
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// RF transmitter
RH_ASK rf_driver(4000, -1, TX_PIN, -1, false);

const char MSG[] = "Group170Press";
const uint8_t MSG_LEN = 13;

bool lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long DEBOUNCE_DELAY = 50;

unsigned long lastSensorRead = 0;
const unsigned long SENSOR_INTERVAL = 2000;  // Read every 2 seconds

float temperature = 0;
float humidity = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("RF Transmitter with DHT11 Starting");
  
  // Initialize button
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  // Initialize DHT11
  dht.begin();
  
  // Initialize OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED init failed!");
  } else {
    Serial.println("OLED ready");
  }
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("RF Transmitter");
  display.println("Ready!");
  display.display();
  delay(1000);
  
  // Initialize RF transmitter
  if (!rf_driver.init()) {
    Serial.println("RF init FAILED!");
  } else {
    Serial.println("RF transmitter ready");
  }
}

void loop() {
  // Read DHT11 sensor periodically
  if (millis() - lastSensorRead > SENSOR_INTERVAL) {
    humidity = dht.readHumidity();
    temperature = dht.readTemperature();
    
    if (isnan(humidity) || isnan(temperature)) {
      Serial.println("DHT11 read failed!");
    } else {
      Serial.print("Temp: ");
      Serial.print(temperature);
      Serial.print("°C, Humidity: ");
      Serial.print(humidity);
      Serial.println("%");
    }
    
    updateDisplay(false);
    lastSensorRead = millis();
  }
  
  // Check button
  int buttonState = digitalRead(BUTTON_PIN);
  
  if (buttonState == LOW && lastButtonState == HIGH) {
    if (millis() - lastDebounceTime > DEBOUNCE_DELAY) {
      
      Serial.println("Button pressed - sending message");
      updateDisplay(true);
      
      // Send message multiple times for reliability
      for (int i = 0; i < 5; i++) {
        rf_driver.send((uint8_t*)MSG, MSG_LEN);
        rf_driver.waitPacketSent();
        delay(10);
      }
      
      Serial.println("Message sent");
      lastDebounceTime = millis();
      
      delay(200);  // Show "SENT" message briefly
      updateDisplay(false);
    }
  }
  
  lastButtonState = buttonState;
  delay(10);
}

void updateDisplay(bool sending) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  
  // Title
  display.setCursor(0, 0);
  display.println("RF Transmitter");
  display.println("---------------");
  
  // Temperature and Humidity
  display.setCursor(0, 20);
  display.print("Temp: ");
  if (!isnan(temperature)) {
    display.print(temperature, 1);
    display.println(" C");
  } else {
    display.println("--");
  }
  
  display.setCursor(0, 32);
  display.print("Humid: ");
  if (!isnan(humidity)) {
    display.print(humidity, 1);
    display.println(" %");
  } else {
    display.println("--");
  }
  
  // Status message
  display.setCursor(0, 50);
  if (sending) {
    display.setTextSize(2);
    display.println("SENT!");
  } else {
    display.println("Press button...");
  }
  
  display.display();
}
