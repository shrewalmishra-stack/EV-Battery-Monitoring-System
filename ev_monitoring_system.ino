#include <WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal_I2C.h>

// -------- Pin Definitions --------
#define VOLTAGE_PIN 34
#define TEMP_PIN 4
#define BUZZER_PIN 5

// -------- Objects --------
OneWire oneWire(TEMP_PIN);
DallasTemperature sensors(&oneWire);

// 16 columns, 2 rows
LiquidCrystal_I2C lcd(0x27, 16, 2);

// -------- Battery Limits (48V Battery Example) --------
float minVolt = 42.0;
float maxVolt = 54.6;

void setup() {

  Serial.begin(115200);
  sensors.begin();
  pinMode(BUZZER_PIN, OUTPUT);

  lcd.init();
  lcd.backlight();

  // Welcome Screen
  lcd.setCursor(0, 0);
  lcd.print("EV Monitoring");
  lcd.setCursor(0, 1);
  lcd.print("System Starting");
  delay(2000);
  lcd.clear();
}

void loop() {

  // -------- Voltage Reading --------
  int rawValue = analogRead(VOLTAGE_PIN);
  float voltage = rawValue * (3.3 / 4095.0) * 15;  

  // Battery %
  float percentage = ((voltage - minVolt) / (maxVolt - minVolt)) * 100;
  if (percentage > 100) percentage = 100;
  if (percentage < 0) percentage = 0;

  // -------- Temperature Reading --------
  sensors.requestTemperatures();
  float temperature = sensors.getTempCByIndex(0);

  // -------- Alert Conditions --------
  bool alert = false;
  String status = "HIGH";

  if (voltage < 44.0) {
    status = "LOW BAT";
    alert = true;
  }

  if (voltage > 54.6) {
    status = "OVR VOLT";
    alert = true;
  }

  if (temperature > 50.0) {
    status = "OVERHEAT";
    alert = true;
  }

  digitalWrite(BUZZER_PIN, alert ? HIGH : LOW);

  // -------- Serial Monitor --------
  Serial.println("------ EV Data ------");
  Serial.print("Voltage: ");
  Serial.print(voltage);
  Serial.println(" V");

  Serial.print("Battery: ");
  Serial.print(percentage);
  Serial.println(" %");

  Serial.print("Temp: ");
  Serial.print(temperature);
  Serial.println(" C");

  Serial.print("Status: ");
  Serial.println(status);
  Serial.println("---------------------\n");

  // -------- LCD DISPLAY --------
  lcd.clear();

  // First Row
  lcd.setCursor(0, 0);
  lcd.print("V:");
  lcd.print(voltage, 1);
  lcd.print(" ");
  lcd.print(percentage, 0);
  lcd.print("%");

  // Second Row
  lcd.setCursor(0, 1);
  lcd.print("T:");
  lcd.print(temperature, 1);
  lcd.print("C ");
  lcd.print(status);

  delay(2000);
}
