#include <ESP8266WiFi.h>
#include "ThingSpeak.h"
#include "DHT.h"

const char* ssid = "SAMSUNGPHAT";
const char* password = "1234567890";
unsigned long channelID = 2339889;
const char* writeKey = "VZZ1W4YF07C25T7M";

unsigned long lastTime = 0;
unsigned long timerDelay = 15000;

#define DHTPIN D4
#define DHTTYPE DHT11

WiFiClient client;
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  connectToWiFi();
  ThingSpeak.begin(client);
  dht.begin();
}

void loop() {
  if (millis() - lastTime > timerDelay) {
    readAndUploadData();
    lastTime = millis();
  }
}

void connectToWiFi() {
  Serial.print("Connecting to WiFi");
  WiFi.mode(WIFI_STA);

  while (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, password);
    delay(5000);
    Serial.print(".");
  }

  Serial.println("\nConnected to WiFi");
}

void readAndUploadData() {
  if (WiFi.status() != WL_CONNECTED) {
    connectToWiFi();
  }

  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print("%  Temperature: ");
  Serial.print(temperature);
  Serial.println("°C ");

  ThingSpeak.setField(1, temperature);
  ThingSpeak.setField(2, humidity);

  int response = ThingSpeak.writeFields(channelID, writeKey);
  if (response == 200) {
    Serial.println("Channel update successful.");
  } else {
    Serial.println("Problem updating channel. HTTP error code " + String(response));
  }
}
