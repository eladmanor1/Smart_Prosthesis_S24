#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "Smart_Prosthesis";       
const char* password = "100inIOT";
// The default IP within the access point to communicate with the hand is 192.168.4.1  
const char* serverName = "http://192.168.4.1/sensor_data"; 
#define ROTATION_SENSOR_PIN 33  // Use GPIO 33 or any other available GPIO pin


void connect_to_wifi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void setup() {
  Serial.begin(115200);
  delay(7000);
  pinMode(ROTATION_SENSOR_PIN, INPUT);
  connect_to_wifi();
}

// This function adapts the payload for the server.
// When sending a payload to the hand from a sensor, please use this function.
void send_sensor_values(uint8_t *payload, int payload_len) {
    String payloadStr = "";
    for (size_t i = 0; i < payload_len; i++) {
      if (i > 0) payloadStr += ",";
      Serial.print("payload [");
      Serial.print(i);
      Serial.print("]: ");
      Serial.println(payload[i]);
      payloadStr += String(payload[i]);
    }

    HTTPClient http;
    http.begin(serverName);
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
}

// In this example, we send a sensor value every 2 seconds, with sensor id = 1.
uint8_t value = 0;
int sensorValue = 0;
void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    uint8_t id = 1;
    // First byte of the payload is always the sensor id, followed by the sensor values.
    value++;
    sensorValue = analogRead(ROTATION_SENSOR_PIN);
    Serial.print("Sensor Value before: ");
    Serial.println(sensorValue);
    sensorValue = (int)map(sensorValue, 0, 4095, 0, 255);
    Serial.print("Sensor Value after: ");
    Serial.println(sensorValue);
    uint8_t payload[2] = {id, (uint8_t)sensorValue};
    send_sensor_values(payload, sizeof(payload));
    
  } else {
    Serial.println("WiFi Disconnected. Attempting to reconnect...");
    connect_to_wifi();
  }
  delay(2000); // Send data every second
}
