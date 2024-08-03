#ifndef HAND_FUNCTIONS
#define HAND_FUNCTIONS
#include "classes.h"
#include <map>
#include <vector>
#include <ESP32Servo.h>

extern Hand* hand;
extern SemaphoreHandle_t xMutex_state;
// -------------------------------------------------------------------------------------------------------------------------------- // 
// ------------------------------------------------- functions --------------------------------------------------------------- // 
// -------------------------------------------------------------------------------------------------------------------------------- // 



/* --------- func: sensor_1_func sub functions --------------- */

void finger_1_up_to_90_fast(Servo* my_servo , const uint8_t angle){
      for (int i = 0; i < 8; i++) {
        my_servo->write(angle);  // Set the servo to the specified angle
        delay(500);            // Wait for 0.5 seconds to allow the servo to reach the position
        my_servo->write(0);              // Set the servo to the specified angle
        delay(500); 
    }
}

void finger_1_more_than_90_slow(Servo* my_servo , const uint8_t angle){
      for (int i = 0; i < 2; i++) {
        my_servo->write(angle);  // Set the servo to the specified angle
        delay(1500);
        my_servo->write(0);              // Set the servo to the specified angle
        delay(1500);          // Wait for 0.5 seconds to allow the servo to reach the position
    }  
 
}

// /* --------- func: sensor_1_func main function --------------- */
// void sensor_1_func(std::vector<String, double> params, const uint8_t *payload) {
//   uint8_t sensor_id = payload[0];
//   uint8_t sensor_value = payload[1];
//   Serial.print("Got sensor id: ");
//   Serial.println(sensor_id);
//   Serial.print("With value: ");
//   Serial.println(sensor_value);

//   Servo_motor* finger1_servo = (Servo_motor*)hand->get_output_by_name("finger1_servo");
//   int control_pin = finger1_servo->control_pin;
//   Servo my_servo;
//   my_servo.attach(control_pin);
//   if(sensor_value > 0 && sensor_value < 90){
//     finger_1_up_to_90_fast(&my_servo , sensor_value);
//   }
//   if(sensor_value > 90 && sensor_value < 180){
//     finger_1_more_than_90_slow(&my_servo , sensor_value);
//   }
// }

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
  int in1_pin = finger1_dc->in1_pin;
  int in2_pin = finger1_dc->in2_pin;
  int sense_pin = finger1_dc->sense_pin;

  pinMode(in1_pin, OUTPUT);
  pinMode(in2_pin, OUTPUT);
  int motor_current = analogRead(sense_pin);
  Serial.print("motor_current : ");
  Serial.println(motor_current);
  if (xSemaphoreTake(xMutex_state, portMAX_DELAY)){
    hand->hand_state = CLOSING_HAND;
    xSemaphoreGive(xMutex_state);
  }
  
  if(sensor_value == 10){
    // Rotate motor clockwise
    digitalWrite(in1_pin, HIGH);
    digitalWrite(in2_pin, LOW);
  }
  else {
    // Rotate motor counterclockwise
    digitalWrite(in1_pin, LOW);
    digitalWrite(in2_pin, HIGH);
  }
}

std::map<String, FuncPtr> func_map = {
  {"sensor_1_func", sensor_1_func}
};


// -------------------------------------------------------------------------------------------------------------------------------- // 
// ------------------------------------------------- end conditions --------------------------------------------------------------- // 
// -------------------------------------------------------------------------------------------------------------------------------- // 
void check_end_conditions(std::map<String , double> currents){
  enum state hand_state;
  if (xSemaphoreTake(xMutex_state, portMAX_DELAY)){
    DC_motor* finger1_motor;
    DC_motor* finger2_motor;
    switch(hand->hand_state){
      case (CLOSING_HAND):
        finger1_motor =(DC_motor*)hand->get_output_by_name("finger1_dc");
        if(!finger1_motor) break;
        if(currents["finger1_dc"] > finger1_motor->threshold){
          digitalWrite(finger1_motor->in1_pin, LOW);
          digitalWrite(finger1_motor->in2_pin, LOW);
          hand->hand_state=INITAIL_STATE;
        }
        break;
      case (OPENIING_HAND):
        finger2_motor =(DC_motor*)hand->get_output_by_name("finger2_dc");
        if(!finger2_motor) break;
        if(currents["finger2_dc"] > finger2_motor->threshold){
          digitalWrite(finger2_motor->in1_pin, LOW);
          digitalWrite(finger2_motor->in2_pin, LOW);
          hand->hand_state=INITAIL_STATE;
        }
        break;
    }
      xSemaphoreGive(xMutex_state);
  }
}
      
#endif /* HAND_FUNCTIONS */