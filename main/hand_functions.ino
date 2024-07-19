#ifndef HAND_FUNCTIONS
#define HAND_FUNCTIONS
#include "classes.h"
#include <map>
#include <vector>

void motor_func(std::vector<double> params, const uint8_t *payload) {
  Serial.print("Got sensor id: ");
  Serial.println(payload[0]);
  Serial.print("With value: ");
  Serial.println(payload[1]);
  for (double param : params) {
    Serial.print("param: ");
    Serial.println(param);
  }
}

std::map<String, FuncPtr> func_map = {
  {"motor_func", motor_func}
};


#endif /* HAND_FUNCTIONS */