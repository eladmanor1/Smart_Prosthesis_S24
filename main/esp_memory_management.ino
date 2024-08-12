#ifndef ESP_MEMORY_MANAGEMENT
#define ESP_MEMORY_MANAGEMENT

#include <Preferences.h>

extern Hand* hand;
extern String yaml_configs;
extern Preferences preference;


/**
 * @brief Loads the saved configurations from non-volatile memory.
 * 
 * This function retrieves the YAML configuration string from the non-volatile memory using the `Preferences` library. If a saved configuration
 * is found, it is converted from YAML to JSON and applied to the system using `yaml_to_json`. If no configuration is found, a message is printed
 * to indicate this.
 */
void load_configs(){
  preference.begin("saved_configs", false);
  yaml_configs = preference.getString("yaml_configs", "empty");
  if(yaml_configs != "empty"){
    Serial.println("found configurations");
    yaml_to_json(yaml_configs.c_str(), false);
  } else {
    Serial.println("didn't find old configurations");
  }
  preference.end();
}

/**
 * @brief Stores the current configurations to non-volatile memory.
 * 
 * This function saves the current YAML configuration string to non-volatile memory using the `Preferences` library. This allows the configurations
 * to be preserved across power cycles.
 */
void store_configs(){
  preference.begin("saved_configs", false);
  preference.putString("yaml_configs", yaml_configs);
  preference.end();
}

#endif


