#ifndef YAML_TO_JSON_PARSER
#define YAML_TO_JSON_PARSER
#include <Arduino.h>
#include <ArduinoJson.h>
#include <YAMLDuino.h>
#include <string>
#include <vector>
#include "hand_functions.ino"
extern Hand* hand;

void config_system(JsonDocument doc) {
  JsonArray input_list = doc["inputs"].as<JsonArray>();

  for (JsonVariant value : input_list) {
    const char *input_name = value["name"];
    Serial.print("input_name: ");
    Serial.println(input_name);
    int id = value["id"];
    Serial.print("id: ");
    Serial.println(id);
    const char *sensor_type = value["type"];
    Serial.print("sensor_type: ");
    Serial.println(sensor_type);
    const char *func_name = value["function"]["name"];
    Serial.print("func_name: ");
    Serial.println(func_name);
    JsonArray func_params_list = value["function"]["parameters"].as<JsonArray>();
    std::vector<double> func_params;
    for (JsonVariant param : func_params_list) {
      func_params.push_back((double)param);
      Serial.print("prarm: ");
      Serial.println((int)param);
    }
    String str_func_name(func_name);
    FuncPtr sensor_func = func_map[str_func_name];
    if(sensor_func == NULL){
      Serial.print("error! coulndt find the pointer to function:");
      Serial.println(str_func_name);
    }
    Func_of_input func_of_input(sensor_func, func_params);
    Sensor *sensor = new Sensor(id, input_name, func_of_input);
    hand->add_input(sensor);
  }

  JsonArray output_list = doc["outputs"].as<JsonArray>();
  for (JsonVariant value : output_list) {
    const char *output_name = value["name"];
    const char *output_type = value["type"];
    if (strcmp(output_type, "servo_motor") == 0) {
      JsonArray pins_list = value["pins"].as<JsonArray>();
      int control_pin;
      for (JsonVariant pin : pins_list) {
        const char *pin_type = pin["type"];
        if (strcmp(pin_type, "control") == 0) {
          control_pin = pin["pin_number"];
        }
      }
      Servo_motor *servo_motor = new Servo_motor(output_name, output_type, control_pin);
      hand->add_output(servo_motor);
    } else if (strcmp(output_type, "DC_motor") == 0) {
        // add logic
    }
  }
}

void yaml_to_json(const char *yaml_str) {
  JsonDocument doc;
  DeserializationError error = deserializeYml(doc, yaml_str);
  if (error) {
    Serial.print(F("deserializeYml() failed: "));
    Serial.println(error.f_str());
    return;
  }
  const char *file_type = doc["file_type"];
  Serial.print("file type: ");
  Serial.println(file_type);
  if (strcmp(file_type, "config_system") == 0) {
    config_system(doc);
  } else if (file_type == "debug_mode") {
    // add debug function
  } else {
    Serial.print("Received unknown file type: ");
    Serial.println(file_type);
  }
}

#endif /* YAML_TO_JSON_PARSER */