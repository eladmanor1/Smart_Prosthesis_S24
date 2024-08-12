#ifndef MAIN
#define MAIN
#include "wifi_communication.ino"
#include "BLE_communication.ino"
#include "yaml_to_json_parser.ino"
#include "esp_memory_management.ino"

#define STACK_SIZE 2048
Preferences preference;
Hand* hand;

// -------- globals -------------//
SemaphoreHandle_t xMutex_state = NULL;
SemaphoreHandle_t xMutex_payload = NULL;
volatile bool is_semaphore_being_deleted = false; // Flag to indicate deletion
Received_command cmd;
// ------------------------------------ //

/**
 * @brief Processes sensor payloads and manages logic based on sensor commands.
 * 
 * This function continuously checks for pending commands in the `cmd` object. When a pending command is detected,
 * it retrieves the sensor by ID, updates the sensor's timestamp, executes the associated function with the command payload,
 * and then marks the command as processed. The function uses a semaphore to ensure mutual exclusion while accessing 
 * the shared `cmd` object.
 * 
 * @param pvParameters Unused parameter.
 */
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



/**
 * @brief Manages hardware states and executes motor actions based on current readings.
 * 
 * This function periodically reads current values from DC motors and filters them using a specified ratio. It then calls 
 * `HW_execute` to execute the appropriate actions based on the current readings. The function initializes a map of currents 
 * for the motors and updates them in a loop. The loop includes a short delay to balance processing.
 * 
 * @param pvParameters Unused parameter.
 */
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

TaskHandle_t hw_Management_Handle = NULL;
TaskHandle_t process_Logic_Handle = NULL;


/**
 * @brief Initializes system components and creates tasks for hardware management and logic processing.
 * 
 * The `setup` function sets up the serial communication, initializes the Wi-Fi server and BLE, creates a `Hand` instance,
 * loads configuration files, creates semaphores for mutual exclusion, and creates tasks for `HW_management` and
 * `process_payload_and_manage_logic` functions.
 */
void setup() {
  Serial.begin(115200);
  delay(5000);
  bring_up_wifi_server();
  init_BLE();
  hand = new Hand();
  load_configs();
  xMutex_state = xSemaphoreCreateMutex();
  xMutex_payload = xSemaphoreCreateMutex();
  xTaskCreate(HW_management, "HW_management", STACK_SIZE ,NULL ,1, &hw_Management_Handle);
  xTaskCreate(process_payload_and_manage_logic , "process_payload_and_manage_logic", STACK_SIZE ,NULL ,1, &process_Logic_Handle);
}

/**
 * @brief Main loop function that handles client connections and toggles an LED.
 * 
 * In the `loop` function, an LED connected to pin 2 is toggled based on the system's uptime. It also continuously
 * handles client requests via the `server` object.
 */
void loop() {
  digitalWrite(2, millis() / 1000 % 2 == 0 ? HIGH : LOW);
  server.handleClient();
}

#endif /* MAIN */