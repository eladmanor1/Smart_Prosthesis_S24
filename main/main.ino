#ifndef MAIN
#define MAIN
#include "wifi_communication.ino"
#include "BLE_communication.ino"
#include "yaml_to_json_parser.ino"
#include "esp_memory_management.ino"



#define STACK_SIZE 2048
Preferences preference;
Hand* hand;
bool processing_command = false;
void check_end_conditions();

void process_payload_and_execute_command(const uint8_t* command_payload) {
  if (!processing_command) {
    processing_command = true;
    int id = command_payload[0];
    Sensor* sensor = (Sensor*)(hand->get_input_by_id(id));
    if (sensor) {
      sensor->last_signal_timestamp = millis();
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

void motors_senses_polling(void* pvParameters){
    std::map<String , double> currents;
    for (Output* output : hand->outputs){
      if(output->type == "DC_motor"){ //initailization 
        currents[output->name] = 0;
      }
    }
    while(1){
      for (Output* output : hand->outputs){
        if(output->type == "DC_motor"){ 
          DC_motor* motor_ptr = (DC_motor*)output;
          currents[motor_ptr->name] = (currents[motor_ptr->name])*0.85 + analogRead(motor_ptr->sense_pin)*0.15;
          if((int)currents[motor_ptr->name] != 0){
            Serial.print("Got current: ");
            Serial.println(currents[motor_ptr->name] );
          }

        }
      }
      check_end_conditions(currents);
    }
}

void setup() {
  Serial.begin(115200);
  delay(10000);
  bring_up_wifi_server();
  init_BLE();
  hand = new Hand();
  load_configs();
  xTaskCreate(motors_senses_polling , "motors_senses_polling", STACK_SIZE ,NULL ,1, NULL);
}

void loop() {
  digitalWrite(2, millis() / 1000 % 2 == 0 ? HIGH : LOW);
  server.handleClient();
}

#endif /* MAIN */