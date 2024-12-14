#include <WiFi.h>
#include <ThingSpeak.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// LCD setup
LiquidCrystal_I2C lcd(0x27, 16, 2); // Set the LCD address to 0x27 for a 16x2 display

// DHT sensor setup
#define DHTPIN 15    // Pin connected to the DHT11 data pin
#define DHTTYPE DHT11  // DHT 11 sensor
DHT dht(DHTPIN, DHTTYPE);

// Analog pins for LDR and MQ3 sensors
#define LDR_PIN 34
#define MQ3_PIN 35

// Buzzer setup
#define BUZZER_PIN 8 // Digital pin connected to the buzzer

// Wi-Fi credentials
const char *ssid = "vaishnavi";         // Replace with your Wi-Fi network name
const char *password = "vaishnavi";    // Replace with your Wi-Fi password

// ThingSpeak settings
unsigned long myChannelNumber = 2785305; // Replace with your ThingSpeak channel number
const char *myWriteAPIKey = "R5HFP0V5S42FT5PO"; // Replace with your ThingSpeak Write API key
WiFiClient client;

void setup() {
  // Initialize LCD
  lcd.init();
  lcd.backlight();

  // Initialize DHT sensor
  dht.begin();

  // Begin serial communication for debugging
  //Serial.begin(115200);
  Serial.begin(115200);

  // Initialize buzzer pin
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW); // Start with the buzzer off

  // Display a welcome message
  lcd.setCursor(0, 0);
  lcd.print("Sensor System");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  delay(2000); // Wait for 2 seconds

  // Connect to Wi-Fi
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connecting WiFi...");
  Serial.print("Connecting to Wi-Fi");

  WiFi.begin(ssid, password);

  int attemptCount = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    lcd.setCursor(0, 1);
    lcd.print("Attempt: ");
    lcd.print(++attemptCount);
    Serial.print(".");

    if (attemptCount > 20) { // Timeout after 20 attempts
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("WiFi Failed");
      lcd.setCursor(0, 1);
      lcd.print("Check Network");
      Serial.println("Wi-Fi connection failed.");
      while (true); // Halt the program
    }
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi Connected");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());
  Serial.println("WiFi connected");
  Serial.println(WiFi.localIP());

  // Initialize ThingSpeak
  ThingSpeak.begin(client);
   Serial.println("ThingSpek connection established");
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

  // Debug: Print sensor values to the serial monitor
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" C, Humidity: ");
  Serial.print(humidity);
  Serial.print("%, LDR Value: ");
  Serial.print(ldrValue);
  Serial.print(", Gas Value: ");
  Serial.println(gasValue);

  // Display sensor values on LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(temperature);
  lcd.print("C H:");
  lcd.print(humidity);
  lcd.setCursor(0, 1);
  lcd.print("LDR:");
  lcd.print(ldrValue);
  lcd.print(" G:");
  lcd.print(gasValue);

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
  } else if (gasValue > 700) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Food Spoiled!");
    Serial.println("ALERT: Food spoiled due to high gas value (>700)");
    digitalWrite(BUZZER_PIN, HIGH); // Activate buzzer
    delay(2000);
    digitalWrite(BUZZER_PIN, LOW); // Deactivate buzzer
  } else if (gasValue > 500) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Warning: Gas");
    lcd.setCursor(0, 1);
    lcd.print("is High");
    Serial.println("Warning: Gas concentration >500");
    delay(2000);
  }

  // Send data to ThingSpeak
  ThingSpeak.setField(1, temperature);
  ThingSpeak.setField(2, humidity);
  ThingSpeak.setField(3, ldrValue);
  ThingSpeak.setField(4, gasValue);
  int httpCode = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if (httpCode == 200) {
    Serial.println("Data sent to ThingSpeak successfully");
  } else {
    Serial.print("Error sending data to ThingSpeak. HTTP code: ");
    Serial.println(httpCode);
  }

  delay(2000); // Wait for 2 seconds before the next update
}
