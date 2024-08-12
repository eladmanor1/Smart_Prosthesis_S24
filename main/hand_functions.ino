#ifndef HAND_FUNCTIONS
#define HAND_FUNCTIONS
#include "classes.h"
#include <map>
#include <vector>

extern Hand* hand;
extern SemaphoreHandle_t xMutex_state;
#define FILTER_RATIO 0.85
// -------------------------------------------------------------------------------------------------------------------------------- // 
// ------------------------------------------------- Admin functions --------------------------------------------------------------- // 
// -------------------------------------------------------------------------------------------------------------------------------- // 

/* --------- func: sensor_1_func main function --------------- */

/**
 * @brief Executes the sensor-specific function for sensor 1.
 * 
 * This function processes the payload data from sensor 1, including sensor ID and value, and adjusts the states of two DC motors
 * based on the sensor value. The function sets the direction and speed of the motors and handles concurrency using a semaphore.
 * 
 * @param params A map containing parameters such as speed and other values used for motor control.
 * @param payload Pointer to the payload data received from the sensor.
 */
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
  DC_motor* finger2_dc = (DC_motor*)hand->get_output_by_name("finger2_dc");
  if (xSemaphoreTake(xMutex_state, portMAX_DELAY)){
    int speed = (int)map(sensor_value, 0, 255, 0, 100);
    if(speed < 50 ){
      finger1_dc->set_state(FORWARD, speed, 15);//(dir,speed (0-100),threshold)
      finger2_dc->set_state(BACKWARD, speed, 15);//(dir,speed (0-100),threshold)
    }
    else {
      finger1_dc->set_state(BACKWARD, speed-50, 15);//(dir,speed (0-100),threshold)
      finger2_dc->set_state(FORWARD, speed-50, 15);//(dir,speed (0-100),threshold)
    }
    xSemaphoreGive(xMutex_state);
  }
}

std::map<String, FuncPtr> func_map = {
  {"sensor_1_func", sensor_1_func}
};

// -------------------------------------------------------------------------------------------------------------------------------- // 
// ------------------------------------------------- end of  Admin functions --------------------------------------------------------------- // 
// -------------------------------------------------------------------------------------------------------------------------------- // 

/**
 * @brief Controls the DC motor based on the specified speed and direction.
 * 
 * This function writes to the motor pins to set the motor's speed and direction. It handles forward, backward, and stop states
 * for the motor and uses PWM for speed control.
 * 
 * @param motor_ptr Pointer to the DC motor object to be controlled.
 * @param speed The speed to set for the motor, scaled from 0 to 255.
 */
void write_to_motor(DC_motor* motor_ptr , int speed){
  int in1_pin = motor_ptr->in1_pin;
  int in2_pin = motor_ptr->in2_pin;
  if (motor_ptr->state.dir == FORWARD){          
    analogWrite(in1_pin, speed);
    digitalWrite(in2_pin, LOW);
  }
  if (motor_ptr->state.dir == BACKWARD){
    digitalWrite(in1_pin, LOW);
    analogWrite(in2_pin, speed);
  }
  if (motor_ptr->state.dir == STOP){ // this is the new check_end_conditions
    digitalWrite(in1_pin, LOW);
    digitalWrite(in2_pin, LOW);
  }
}


/**
 * @brief Executes hardware actions based on the current sensor readings.
 * 
 * This function iterates through all DC motors, updates their state based on current readings, and writes the appropriate control
 * signals to the motors. It checks if the current exceeds a custom threshold to stop the motor if necessary, and updates the
 * motor's state accordingly. Concurrency is managed using a semaphore.
 * 
 * @param currents A map containing the current readings for each motor, which are used to adjust motor states.
 */
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
        if(currents[motor_ptr->name] > motor_ptr->state.custom_threshold){ //polling on the currents, and check if stop is needed.
            motor_ptr->set_state(STOP,0,motor_ptr->state.custom_threshold);
        }
        int speed = (int)map(motor_ptr->state.speed, 0, 100, 0, 255);
        write_to_motor(motor_ptr ,speed);
        xSemaphoreGive(xMutex_state);
      }
    }
  }
}

    
#endif /* HAND_FUNCTIONS */