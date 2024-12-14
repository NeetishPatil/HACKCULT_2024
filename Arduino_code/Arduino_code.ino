#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// LCD setup
LiquidCrystal_I2C lcd(0x27, 16, 2); // Set the LCD address to 0x27 for a 16x2 display

// DHT sensor setup
#define DHTPIN A0    // Pin connected to the DHT11 data pin (update this pin to match your setup)
#define DHTTYPE DHT11  // DHT 11 sensor
DHT dht(DHTPIN, DHTTYPE);

// Analog pins for LDR and MQ3 sensors
#define LDR_PIN A1
#define MQ3_PIN A2

// Buzzer setup
#define BUZZER_PIN 8 // Digital pin connected to the buzzer

void setup() {
  // Initialize LCD
  lcd.init();
  lcd.backlight(); // Turn on the backlight

  // Initialize DHT sensor
  dht.begin();

  // Begin serial communication for debugging
  Serial.begin(9600);

  // Initialize buzzer pin
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW); // Start with the buzzer off

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

  // Validate sensor readings to avoid incorrect conditions
  if (isnan(temperature) || isnan(humidity)) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Sensor Error!");
    Serial.println("Error: DHT11 sensor not responding.");
    digitalWrite(BUZZER_PIN, HIGH); // Activate buzzer for sensor error
    delay(2000);
    digitalWrite(BUZZER_PIN, LOW); // Turn off buzzer
    return; // Skip further checks
  }

  // Debug: Print sensor values to the serial monitor for debugging
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" C, Humidity: ");
  Serial.print(humidity);
  Serial.print("%, LDR Value: ");
  Serial.print(ldrValue);
  Serial.print(", Gas Value: ");
  Serial.println(gasValue);

  // Check for warning conditions
  if (humidity > 300) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Warning: Humidity");
    lcd.setCursor(0, 1);
    lcd.print(">300%");
    Serial.println("Warning: Humidity too high (>300%)");
    delay(2000);
  } else if (temperature > 40) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Warning: Temp");
    lcd.setCursor(0, 1);
    lcd.print(">40C");
    Serial.println("Warning: Temperature too high (>40°C)");
    delay(2000);
  } else if (gasValue > 700) { // Adjusted threshold for food spoilage warning
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Food Spoiled!");
    Serial.println("ALERT: Food spoiled due to high gas value (>700)");
    digitalWrite(BUZZER_PIN, HIGH); // Activate buzzer
    delay(2000);
    digitalWrite(BUZZER_PIN, LOW); // Deactivate buzzer
  } else if (gasValue > 400) { // Adjusted threshold for gas warning
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Warning: Gas");
    lcd.setCursor(0, 1);
    lcd.print("is High");
    Serial.println("Warning: Gas concentration >400");
    delay(2000);
  } else {
    // Normal operation display
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("T:");
    lcd.print(temperature);
    lcd.print("C H:");
    lcd.print(humidity);
    lcd.print("%");

    lcd.setCursor(0, 1);
    lcd.print("LDR:");
    lcd.print(ldrValue);
    lcd.print(" G:");
    lcd.print(gasValue);

    delay(2000); // Wait for 2 seconds before the next update
  }
}
