#ifndef MAIN
#define MAIN
#include "wifi_communication.ino"
#include "BLE_communication.ino"
#include "yaml_to_json_parser.ino"
#include "esp_memory_management.ino"

Preferences preference;
Hand* hand;
bool processing_command = false;

void process_payload_and_execute_command(const uint8_t* command_payload) {
  if (!processing_command) {
    processing_command = true;
    int id = command_payload[0];
    Sensor* sensor = (Sensor*)(hand->get_input_by_id(id));
    if (sensor) {
      Serial.print("Got sensor: ");
      Serial.println(sensor->name);
      sensor->func_of_input_obj.execute_func(command_payload);
    } else {
      Serial.print("Didn't find sensor by id: ");
      Serial.println(id);
    }
    processing_command = false;
  }
}

void setup() {
  Serial.begin(115200);
  delay(10000);
  bring_up_wifi_server();
  init_BLE();
  hand = new Hand();
  load_configs();
}

void loop() {
  digitalWrite(2, millis() / 1000 % 2 == 0 ? HIGH : LOW);
  server.handleClient();
}

#endif /* MAIN */