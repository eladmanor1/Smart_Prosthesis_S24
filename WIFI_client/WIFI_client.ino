#include <WiFi.h>
#include <HTTPClient.h>
#include <Base64.h>

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

uint8_t value = 0;
void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverName);

    uint8_t id = 1;
    uint8_t payload[2] = {id, value};
    value+=1;
    String payloadStr = "";
    for (size_t i = 0; i < sizeof(payload); i++) {
      if (i > 0) payloadStr += ",";
      Serial.println(payload[i]);
      payloadStr += String(payload[i]);
    }
    http.addHeader("Content-Type", "application/octet-stream");
    int httpResponseCode = http.POST(payloadStr);

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
  if(value > 20){
    value=0;
  }
}
