#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>   // Include the DHT library

#define DHTPIN 2          
#define DHTTYPE DHT11     

LiquidCrystal_I2C lcd(0x27,16,2);
DHT dht(DHTPIN, DHTTYPE);

void setup() 
{
  Serial.begin(9600); // Initialize the Serial communication
  lcd.begin();
  dht.begin();
}

void loop() {
  float Humidity = dht.readHumidity();
  float Temperature = dht.readTemperature();

  if (isnan(Humidity) || isnan(Temperature)) {
    lcd.setCursor(0, 1);
    lcd.print("Failed to read from DHT");
    Serial.println("Failed to read from DHT");  // Print to Serial Monitor
    return;
  }

  lcd.setCursor(0, 0);
  lcd.print("Hum: ");
  lcd.print(Humidity);
  lcd.print("%");

  lcd.setCursor(0, 1);
  lcd.print("Temp: ");
  lcd.print(Temperature);
  lcd.print(" C");

  // Send the data to Serial Monitor
  Serial.print("Hum: ");
  Serial.print(Humidity);
  Serial.print("%   Temp: ");
  Serial.print(Temperature);
  Serial.println(" C");

  delay(2000);  
}