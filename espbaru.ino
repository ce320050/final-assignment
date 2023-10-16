#include <ArduinoJson.h>
#include <WiFi.h>
#include <HTTPClient.h>

#define RX2 16
#define TX2 17

#define ssid "Jeremi"
#define password "sadasampetolu"
#define host "weebsite.online"

WiFiClient wifiClient;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial2.begin(9600);
  
  if (!WiFi.begin(ssid, password)) {
    Serial.println("Failed to connect to WiFi");
    while (1);
  }

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
}

void loop() {
  if (Serial2.available()) {
    String message = Serial2.readString();
    Serial.println(message);

    StaticJsonDocument<100> doc;
    DeserializationError error = deserializeJson(doc, message);

    if (error) {
      Serial.println("Failed to parse JSON");
      return;
    }

    int suhu = doc["suhu"];
    int ntu = doc["ntu"];
    int ph = doc["ph"];
    int Do = doc["do"];

    String Link;
    HTTPClient http;

    Link = "http://" + String(host) + "/kirimdata.php?suhu=" + String(suhu) + "&ntu=" + String(ntu) + "&ph=" + String(ph) + "&do=" + String(Do);

    
    http.begin(wifiClient, Link);
    int httpCode = http.GET();

    if (httpCode > 0) {
      String response = http.getString();
      Serial.println(response);
    }
    else {
      Serial.println("HTTP request failed");
    }

    http.end();
  }
}
