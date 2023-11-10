#include <ESP8266WiFi.h>
#include <DHT.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "SAMSUNGPHAT";
const char* password = "1234567890";
const char* serverAddress = "http://192.168.4.9:3000/sensors";

WiFiClient client;
HTTPClient http;
DHT dht(D4, DHT11);

unsigned long lastTime = 0;
const unsigned long timerDelay = 15000;

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  connectToWiFi();
  dht.begin();
}

void loop() {
  if (millis() - lastTime > timerDelay) {
    readAndSendData();
    lastTime = millis();
  }
}

void connectToWiFi() {
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected");
    Serial.print("IP address:\t");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nFailed to connect to WiFi. Check your credentials or restart the ESP.");
    // You might want to add a retry mechanism or other error handling here
  }
}

void readAndSendData() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor");
    return;
  }

  Serial.print("Humidity: ");
  Serial.println(humidity);
  Serial.print("Temperature: ");
  Serial.println(temperature);

  // Create a JSON document
  DynamicJsonDocument jsonDocument(200);
  jsonDocument["hum"] = humidity;
  jsonDocument["temp"] = temperature;
  jsonDocument["timestamp"] = millis();

  // Serialize the JSON document to a string
  String jsonData;
  serializeJson(jsonDocument, jsonData);

  // Send data to the server
  sendData(jsonData);
}

void sendData(String data) {

  http.begin(client, serverAddress);
  http.addHeader("Content-Type", "application/json");
  http.setTimeout(10000);

  int httpResponseCode = http.POST(data);

  if (httpResponseCode > 0) {
    Serial.println("HTTP Response code: " + String(httpResponseCode));
  } else {
    Serial.println("Error on sending POST: " + String(httpResponseCode));
  }

  http.end();
}
