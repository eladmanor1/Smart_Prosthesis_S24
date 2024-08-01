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


void process_payload_and_execute_command(const uint8_t* command_payload) {
  if (!processing_command) {
    processing_command = true;
    int id = command_payload[0];
    Sensor* sensor = (Sensor*)(hand->get_input_by_id(id));
    sensor->last_signal_timestamp = millis();
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
          currents[motor_ptr->name] = (currents[motor_ptr->name])*0.9 + analogRead(motor_ptr->sense_pin)*0.1;
          if((int)currents[motor_ptr->name] != 0){
            Serial.print("Got current: ");
            Serial.println(currents[motor_ptr->name] );
          }

        }
      
      }
      //  -------------------------- end conditions for functions -------------------------------------- //
      switch(hand->hand_state){
        case CLOSING_HAND:
          DC_motor* finger1_motor =(DC_motor*)hand->get_output_by_name("finger1_dc");
          if(currents["finger1_dc"] > finger1_motor->threshold){
            digitalWrite(finger1_motor->in1_pin, LOW);
            digitalWrite(finger1_motor->in2_pin, LOW);
            hand->hand_state=INITAIL_STATE;
          }
        break;

      }
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