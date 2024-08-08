#ifndef MAIN
#define MAIN
#include "wifi_communication.ino"
#include "BLE_communication.ino"
#include "yaml_to_json_parser.ino"
#include "esp_memory_management.ino"

#define STACK_SIZE 2048
Preferences preference;
Hand* hand;
void check_end_conditions();

// -------- globals -------------//
SemaphoreHandle_t xMutex_state = NULL;
SemaphoreHandle_t xMutex_payload = NULL;
Received_command cmd;
// ------------------------------------ //


void process_payload_and_manage_logic(void* pvParameters) {
  while(1){
    if(xSemaphoreTake(xMutex_payload, portMAX_DELAY)){
        if (cmd.is_pending){
          int id = cmd.command_payload[0];
          Sensor* sensor = (Sensor*)(hand->get_input_by_id(id));
          if (sensor) {
            sensor->last_signal_timestamp = millis();
            Serial.print("Got sensor: ");
            Serial.println(sensor->name);
            sensor->func_of_input_obj.execute_func(cmd.command_payload);
            cmd.is_pending = false;
          } else {
            Serial.print("Didn't find sensor by id: ");
            Serial.println(id);
          }
          xSemaphoreGive(xMutex_payload);
        } else {
          xSemaphoreGive(xMutex_payload);
          vTaskDelay(100);
        }
        
    }
  }
}

void HW_management(void* pvParameters){
  std::map<String , double> currents;
  for (Output* output : hand->outputs){
    if(output->type == "DC_motor"){ //initailization 
      currents[output->name] = 0;
    }
  }
  while(1){
    vTaskDelay(10);
    for (Output* output : hand->outputs){
      if(output->type == "DC_motor"){ 
        DC_motor* motor_ptr = (DC_motor*)output;
        currents[motor_ptr->name] = (currents[motor_ptr->name])*FILTER_RATIO + analogRead(motor_ptr->sense_pin)*(1-FILTER_RATIO);
        if((int)currents[motor_ptr->name] != 0){
          Serial.print("Got current: ");
          Serial.println(currents[motor_ptr->name] );
        }
      }
    }
    HW_execute(currents);
  }
}

void setup() {
  Serial.begin(115200);
  delay(10000);
  bring_up_wifi_server();
  init_BLE();
  hand = new Hand();
  load_configs();
  xMutex_state = xSemaphoreCreateMutex();
  xMutex_payload = xSemaphoreCreateMutex();
  xTaskCreate(HW_management, "HW_management", STACK_SIZE ,NULL ,1, NULL);
  xTaskCreate(process_payload_and_manage_logic , "process_payload_and_manage_logic", STACK_SIZE ,NULL ,1, NULL);
}

void loop() {
  digitalWrite(2, millis() / 1000 % 2 == 0 ? HIGH : LOW);
  server.handleClient();
}

#endif /* MAIN */