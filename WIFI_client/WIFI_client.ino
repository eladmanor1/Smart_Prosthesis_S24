#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "Smart_Prosthesis_c_elad";       
const char* password = "your_PASSWORD";      
const char* serverName = "http://192.168.4.1/sensor_data"; 

void setup() {
  Serial.begin(115200);
  delay(7000);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverName);

    uint8_t id = 1;
    uint8_t value = 69;
    uint8_t payload[2] = {id, value};

    http.addHeader("Content-Type", "application/octet-stream");
    int httpResponseCode = http.POST(payload, 2);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
  delay(1000); // Send data every second
}
