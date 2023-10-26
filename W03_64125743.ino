#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DHT.h>

// Create a web server on port 80
ESP8266WebServer server(80);

const int ledPin = D6;
DHT dht11(D4, DHT11);

void initWifi(String ssid, String password) {
  
  Serial.println('wifi najaa');
  // Initialize Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println('wifi najaa');

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  initWifi("JaNa", "1234567890");
  
  Serial.println("HTTP server started");
  pinMode(ledPin, OUTPUT);
  dht11.begin();

  // Set up routes
  server.on("/", HTTP_GET, handleRoot);
  server.on("/TurnOn", HTTP_GET, handleTurnOn);
  server.on("/TurnOff", HTTP_GET, handleTurnOff);
  server.on("/data", HTTP_GET, handleData);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}

void handleRoot() {
  float temperature = dht11.readTemperature();
  float humidity = dht11.readHumidity();

  String html = "<html><body><div style='text-align: center;'>";
  html += "<p><h2>Web Server</h2>";
  html += "<p>Humidity: <span id='humidity'>" + String(humidity) + " %</span></p>";
  html += "<p>Temperature: <span id='temperature'>" + String(temperature) + " C</span></p>";
  html += "<p>LED Status: " + (digitalRead(ledPin) == HIGH ? String("<span style='color: #green;'>On</span>") : String("<span style='color: #red;'>Off</span>")) + "</p>";

  html += "<form action='/TurnOn' method='get' style='display: inline;'>";
  html += "<input type='submit' value='Turn LED On' onclick='this.style.backgroundColor=\"#green\"'>";
  html += "</form>";
  html += "<br><br>";
  html += "<form action='/TurnOff' method='get' style='display: inline;'>";
  html += "<input type='submit' value='Turn LED Off' onclick='this.style.backgroundColor=\"#red\"'>";
  html += "</form>";
  html += "</div></body></html>";



  html += "<script>";
  html += "function updateData() {";
  html += "var xhr = new XMLHttpRequest();";
  html += "xhr.open('GET', '/data', true);";
  html += "xhr.onreadystatechange = function() {";
  html += "if (xhr.readyState == 4 && xhr.status == 200) {";
  html += "var data = JSON.parse(xhr.responseText);";
  html += "document.getElementById('temperature').textContent = data.temperature + 'C';";
  html += "document.getElementById('humidity').textContent = data.humidity + '%';";
  html += "}";
  html += "};";
  html += "xhr.send();";
  html += "}";
  html += "setInterval(updateData, 1000);";
  html += "updateData();";
  html += "</script>";

  server.send(200, "text/html", html);
}

void handleTurnOn() {
  digitalWrite(ledPin, HIGH);
  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "");
}

void handleTurnOff() {
  digitalWrite(ledPin, LOW);
  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "");
}

void handleData() {
  float temperature = dht11.readTemperature();
  float humidity = dht11.readHumidity();

  String data = "{\"temperature\": " + String(temperature) + ", \"humidity\": " + String(humidity) + "}";
  server.send(200, "application/json", data);
}
