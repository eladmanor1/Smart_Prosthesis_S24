#include <Arduino.h>
#include <ArduinoJson.h>
#include <YAMLDuino.h>
#include <string>

void config_system(JsonDocument doc) {
  JsonArray input_arr = doc["inputs"].as<JsonArray>();

  for (JsonVariant value : input_arr) {
    const char *name = value["name"];
    Serial.print("name: ");
    Serial.println(name);
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
