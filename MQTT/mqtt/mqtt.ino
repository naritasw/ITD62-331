#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <ArduinoJson.h>

const char* ssid = "Queen";
const char* password = "123456789";
const char* mqtt_server = "192.168.137.1";
const char* mqtt_topic_LED = "LIGHT";
const char* mqtt_topic_DHT11 = "DHT11";

const int DHTPIN = D4;
const int DHTTYPE = DHT11;
const int ledPin1 = D6;

WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHTPIN, DHTTYPE);

void setup_wifi() {
  delay(10);
  Serial.println("Connecting to WiFi");
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nWiFi connected");
  Serial.println("IP address: " + WiFi.localIP().toString());
}

void callback(char* topic, byte* receivedPayload, unsigned int length) {
  Serial.print("Message received on topic: ");
  Serial.println(topic);

  Serial.print("Payload: ");
  String payload = "";

  for (int i = 0; i < length; i++) {
    payload += (char)receivedPayload[i];
    Serial.print((char)receivedPayload[i]);
  }

  Serial.println();

  if (payload.equals("ON")) {
    digitalWrite(ledPin1, HIGH);
    Serial.println("LED turned ON");
  } else if (payload.equals("OFF")) {
    digitalWrite(ledPin1, LOW);
    Serial.println("LED turned OFF");
  } else {
    Serial.println("Invalid payload");
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-" + String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.subscribe(mqtt_topic_LED);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void publishDHTData() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor");
    return;
  }

  StaticJsonDocument<200> doc;
  doc["temperature"] = temperature;
  doc["humidity"] = humidity;

  String payload;
  serializeJson(doc, payload);

  Serial.println("Publishing DHT data: " + payload);
  client.publish(mqtt_topic_DHT11, payload.c_str());
}

void setup() {
  client.setCallback(callback);
  pinMode(ledPin1, OUTPUT);
  digitalWrite(ledPin1, HIGH);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  dht.begin();
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  static unsigned long lastMillis = 0;
  if (millis() - lastMillis > 10000) {
    publishDHTData();
    lastMillis = millis();
  }
}
