#include "wifi_communication.ino"
#include "BLE_communication.ino"
#include "yaml_to_json_parser.ino"

void setup() {
  Serial.begin(115200);
  delay(5000);
  bring_up_wifi_server();
}

void loop() {
  server.handleClient();
  if (configs_ready)
}