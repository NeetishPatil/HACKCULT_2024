#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

#define DHTPIN 4
#define LDR_PIN 36
#define MQ3_PIN 39
// LCD setup
LiquidCrystal_I2C lcd(0x27, 16, 2); // Set the LCD address to 0x27 for a 16x2 display

// DHT sensor setup
#define DHTPIN 36    // Pin connected to the DHT11 data pin
#define DHTTYPE DHT11 // DHT 11 sensor
DHT dht(DHTPIN, DHTTYPE);

// Analog pins for LDR and MQ3 sensors
#define LDR_PIN 39
#define MQ3_PIN A0

void setup() {
  // Initialize LCD
  lcd.init();
  lcd.backlight(); // Turn on the backlight

  // Initialize DHT sensor
  dht.begin();

  // Begin serial communication for debugging (optional)
  Serial.begin(9600);

  // Display a welcome message
  lcd.setCursor(0, 0);
  lcd.print("Sensor System");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  delay(2000); // Wait for 2 seconds
}

void loop() {
  // Read DHT11 sensor data
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Read LDR value
  int ldrValue = analogRead(LDR_PIN);

  // Read MQ3 gas sensor value
  int gasValue = analogRead(MQ3_PIN);

  // Display sensor values on the LCD
  lcd.clear();
  lcd.setCursor(0, 0); // Set cursor to the first row
  lcd.print("T:");
  lcd.print(temperature);
  lcd.print("C H:");
  lcd.print(humidity);
  lcd.print("%");

  lcd.setCursor(0, 1); // Set cursor to the second row
  lcd.print("LDR:");
  lcd.print(ldrValue);
  lcd.print(" G:");
  lcd.print(gasValue);

  // Optional: Print values to Serial Monitor for debugging
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" C");

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  Serial.print("LDR Value: ");
  Serial.println(ldrValue);

  Serial.print("Gas Value: ");
  Serial.println(gasValue);

  delay(2000); // Wait for 2 seconds before the next update
}