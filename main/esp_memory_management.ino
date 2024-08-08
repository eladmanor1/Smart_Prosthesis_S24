#ifndef ESP_MEMORY_MANAGEMENT
#define ESP_MEMORY_MANAGEMENT

#include <Preferences.h>

extern Hand* hand;
extern String yaml_configs;
extern Preferences preference;

void load_configs(){
  preference.begin("saved_configs", false);
  yaml_configs = preference.getString("yaml_configs", "empty");
  if(yaml_configs != "empty"){
    Serial.println("found configurations");
    yaml_to_json(yaml_configs.c_str());
  } else {
    Serial.println("didn't find old configurations");
  }
  preference.end();
}

void store_configs(){
  preference.begin("saved_configs", false);
  preference.putString("yaml_configs", yaml_configs);
  preference.end();
}

#endif


