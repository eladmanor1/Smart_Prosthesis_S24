#ifndef WIFI_CLIENT
#define WIFI_CLIENT

#include <WiFi.h>
#include <HTTPClient.h>

const char* hand_ssid = "your_SSID"; // Replace with your Wi-Fi SSID
const char* hand_password = "your_PASSWORD"; // Replace with your Wi-Fi password
const char* serverName = "http://192.168.4.1/send_command"; // Replace <server_ip> with your server's IP address

unsigned long previousMillis = 0; // Stores last time HTTP request was sent
const long interval = 1000; // Interval at which to send data (milliseconds)

void setup() {
  Serial.begin(115200);
  WiFi.begin(hand_ssid, hand_password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    sendSensorData();
  }
}

void sendSensorData() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverName);

    // Specify content-type header
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    // Prepare your HTTP POST request data
    int id = 1; 
    int sensor_value = random(0, 256); 
    String httpRequestData = "id=" + String(id) + "&sensor_value=" + String(sensor_value);
    
    // Send HTTP POST request
    int httpResponseCode = http.POST(httpRequestData);
    
    // If you need to print the response from the server
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }
    
    http.end(); // Free resources
  } else {
    Serial.println("WiFi Disconnected");
  }
}

#endif


