#ifndef HAND_FUNCTIONS
#define HAND_FUNCTIONS
#include "classes.h"
#include <map>
#include <vector>


extern Hand* hand;
extern SemaphoreHandle_t xMutex_state;
#define FILTER_RATIO 0.85
// -------------------------------------------------------------------------------------------------------------------------------- // 
// ------------------------------------------------- functions --------------------------------------------------------------- // 
// -------------------------------------------------------------------------------------------------------------------------------- // 

/* --------- func: sensor_1_func main function --------------- */
void sensor_1_func(std::map<String, double> params, const uint8_t *payload) {
  uint8_t sensor_id = payload[0];
  uint8_t sensor_value = payload[1];
  Serial.print("Got sensor id: ");
  Serial.println(sensor_id);
  Serial.print("With value: ");
  Serial.println(sensor_value);
  Serial.print("speed : ");
  Serial.println(params["speed"]);
  Serial.print("PARAM 2 : ");
  Serial.println(params["param2"]);
  Serial.print("PARAM 3 : ");
  Serial.println(params["param3"]);

  DC_motor* finger1_dc = (DC_motor*)hand->get_output_by_name("finger1_dc");
  if (xSemaphoreTake(xMutex_state, portMAX_DELAY)){
    if(sensor_value < 10){
      finger1_dc->set_state(FORWARD, 30, 15);
    }
    else {
      finger1_dc->set_state(BACKWARD, 30, 15);
    }
    xSemaphoreGive(xMutex_state);
  }
}

std::map<String, FuncPtr> func_map = {
  {"sensor_1_func", sensor_1_func}
};


// -------------------------------------------------------------------------------------------------------------------------------- // 
// ------------------------------------------------- end conditions --------------------------------------------------------------- // 
// -------------------------------------------------------------------------------------------------------------------------------- // 
void HW_execute(std::map<String , double>& currents){
  for (Output* output : hand->outputs){
    if (xSemaphoreTake(xMutex_state, portMAX_DELAY)){
      if(output->type == "DC_motor"){
        DC_motor* motor_ptr = (DC_motor*)output;
        int in1_pin = motor_ptr->in1_pin;
        int in2_pin = motor_ptr->in2_pin;
        int sense_pin = motor_ptr->sense_pin;
        pinMode(in1_pin, OUTPUT);
        pinMode(in2_pin, OUTPUT);
        currents[motor_ptr->name] = (currents[motor_ptr->name])*FILTER_RATIO + analogRead(motor_ptr->sense_pin)*(1-FILTER_RATIO);
        if(currents[motor_ptr->name] > motor_ptr->state.custom_threshold){
            motor_ptr->set_state(STOP);
        }
        if (motor_ptr->state.dir == FORWARD){
          digitalWrite(in1_pin, HIGH);
          digitalWrite(in2_pin, LOW);
        }
        if (motor_ptr->state.dir == BACKWARD){
          digitalWrite(in1_pin, LOW);
          digitalWrite(in2_pin, HIGH);
        }
        if (motor_ptr->state.dir == STOP){ // this is the new check_end_conditions
          digitalWrite(in1_pin, LOW);
          digitalWrite(in2_pin, LOW);
        }
        xSemaphoreGive(xMutex_state);
      }
    }
  }
}
    
#endif /* HAND_FUNCTIONS */