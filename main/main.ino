#ifndef MAIN
#define MAIN
#include "wifi_communication.ino"
#include "BLE_communication.ino"
#include "yaml_to_json_parser.ino"
#include "esp_memory_management.ino"



Preferences preference;
Hand* hand;

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
  if (configs_waiting)
  {
    yaml_to_json(yaml_configs.c_str());// this func sets the hand objecta ( sensors, motors, etc)
    store_configs();
    configs_waiting = false;
  }
  if (command_received) {
    int id = commandPayload[0];
    Sensor* sensor = (Sensor*)(hand->get_input_by_id(id));
    Serial.print("got sensor:");
    Serial.print(sensor->name);
    if (sensor) {
      sensor->func_of_input_obj.execute_func(commandPayload);
    } else {
      Serial.print("Didn't find sensor by id: ");
      Serial.println(id);
    }
    command_received = false;
  }
}

#endif /* MAIN */