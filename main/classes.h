/**
  @file classes.h
  @brief This header file defines various classes used for a smart prosthesis system. 
 */

#ifndef CLASSES_H
#define CLASSES_H
#include <vector>
#include <String>
#include <map>

// Pointer function that receives a map between strings and doubles (used for fucntion params) and uint8_t (used for payload).
typedef void (*FuncPtr)(std::map<String, double>, const uint8_t*);
#define MAX_PAYLOAD_SIZE 256

/**
 * @enum Direction
 * @brief Enumerates the possible directions for motor movement.
 */
enum Direction{
  STOP,
  FORWARD,
  BACKWARD,
  CLOCKWISE,
  COUNTER_CLOCKWISE
};

/**
 * @class State
 * @brief  base class for motor State. 
 */
class Motor_state {
public:
  enum Direction dir;
  int speed;
  int custom_threshold;
  Motor_state(int custom_threshold, enum Direction dir = STOP, int speed = 0): dir(dir), speed(speed), custom_threshold(custom_threshold){}
};

/**
 * @class input
 * @brief Abstract base class for system elements that can serve as inputs. (motors current / sensors)
 */
class Input {
public:
  int id;
  String name;
  String type;
  unsigned long last_signal_timestamp;
  Input(int id, String name , String type): id(id), name(name),type(type), last_signal_timestamp(0){}
  virtual void debug_print(){
    Serial.println("--INPUT--");
    Serial.print("id : ");
    Serial.println(id);
    Serial.print("name : ");
    Serial.println(name);
    }
  virtual ~Input() {}
};


/**
 * @class output
 * @brief Abstract base class for system elements that can serve as outputs. (motors)
 */
class Output {
public:
  String name;
  String type;
  Output(String name, String type): name(name), type(type){}
  virtual void debug_print(){
    Serial.println("--Output--");
    Serial.print("name : ");
    Serial.println(name);
    Serial.print("type : ");
    Serial.println(type);
    }
  virtual ~Output() {}
};

/**
 * @class Func_of_input
 * @brief class to store the function ptr and the corresponding params.
 */
class Func_of_input {
public:
  FuncPtr func_ptr;
  std::map<String,double> parameters;
  Func_of_input(FuncPtr func_ptr, std::map<String,double> parameters) : func_ptr(func_ptr), parameters(parameters){}

  void execute_func(const uint8_t* sensor_read_value){
    func_ptr(parameters, sensor_read_value);
  }
};

/**
 * @class Sensor
 * @brief Abstract base class for sensors.
 */
class Sensor : public Input{
public:
  Func_of_input func_of_input_obj;
  int in_pin;
  int out_pin;
  bool status;
  friend class Hand;
  Sensor(int id, const String name, const String type, Func_of_input func_of_input, int in_pin=-1, int out_pin=-1) : Input(id, name, type), func_of_input_obj(func_of_input), in_pin(in_pin), out_pin(out_pin), status(false){}
  void debug_print() override{
    Serial.println("--Sensor--");
    Serial.print("id : ");
    Serial.println(id);
    Serial.print("name : ");
    Serial.println(name);
    Serial.print("type : ");
    Serial.println(type);
    Serial.print("in_pin : ");
    Serial.println(in_pin);
    Serial.print("out_pin : ");
    Serial.println(out_pin);
     }
};

/**
 * @class Motor
 * @brief Abstract base class for motors.
 */
class Motor : public Output {
public:
  int safety_threshold;
  Motor_state state;
  int outputRange[2]; 
  Motor(String name, String type, int safety_threshold, const int output_min_val, const int output_max_val)
          : Output(name, type), safety_threshold(safety_threshold), state(Motor_state(safety_threshold)) {
          outputRange[0] = output_min_val;
          outputRange[1] = output_max_val;
  }
  void set_state(enum Direction dir = STOP, int speed = 0, int custom_threshold = 0){
    if (custom_threshold == 0)
      custom_threshold = safety_threshold;
    state.custom_threshold = min(custom_threshold, safety_threshold);
    state.dir = dir;
    state.speed = speed;
  }
};

/**
 * @class Motor
 * @brief class for DC_motors that has two pins used to activate the motor (in_pins) and one to sense the current (sense_pin)
 */
class DC_motor : public Motor {
public:
  int in1_pin;
  int in2_pin;
  int sense_pin;
  DC_motor(String name, String type,  int in1_pin ,  int in2_pin,  int sense_pin , int safety_threshold)
      : Motor(name, type, safety_threshold, LOW, HIGH), in1_pin(in1_pin) , in2_pin(in2_pin) , sense_pin(sense_pin){}
  void debug_print() override{
    Serial.println("--Motor--");
    Serial.print("name : ");
    Serial.println(name);
    Serial.print("type : ");
    Serial.println(type);
    Serial.print("in1_pin : ");
    Serial.println(in1_pin);
    Serial.print("in2_pin : ");
    Serial.println(in2_pin);
    Serial.print("sense_pin : ");
    Serial.println(sense_pin);
    Serial.print("safety_threshold : ");
    Serial.println(safety_threshold);
    }
};

/**
 * @class Received_command
 * @brief class to store the received command payload, length and a flag that indicates if a new command is pending.
 */
class Received_command{
  public:
    uint8_t command_payload[MAX_PAYLOAD_SIZE];
    int command_payload_len;
    bool is_pending;
    Received_command(): command_payload(NULL),command_payload_len(0) , is_pending(false) {}
};

/**
 * @class Hand
 * @brief Represents a hand with multiple motors, sensors, and commands.
 */
class Hand{
public:
  std::vector<Output*> outputs;
  std::vector<Input*> inputs;
  Hand(){}
  void add_output(Output* output){
      outputs.push_back(output);
  }
  void add_input(Input* input){
      inputs.push_back(input);
  }
  void clear_hand(){
    int inputs_size = inputs.size();
    for (int i=0; i < inputs_size; i++){
      Input* input_ptr = inputs.back();  
      delete input_ptr;
      inputs.pop_back();  
    }
    int outputs_size = outputs.size();
    for (int i=0; i < outputs_size; i++){
      Output* output_ptr = outputs.back();  
      delete output_ptr;
      outputs.pop_back();  
    }
  }

  void debug_print(){
    Serial.println(" ---------------------------------");
    Serial.println(" ----- the hand inputs: ---------");
    for (Input* input_ptr : inputs) {
      input_ptr->debug_print();
    }
    Serial.println(" ----- the hand outputs: ---------");
    for (Output* output_ptr : outputs) {
      output_ptr->debug_print();
    }
    Serial.println(" ---------------------------------");
    Serial.println(" ---------------------------------");
  }

  Input* get_input_by_id(int id) {
    for (Input* input_ptr : inputs) {
      if (input_ptr->id == id)
        return input_ptr;
    }
    return NULL;
  }

  Output* get_output_by_name(String name) {
    for (Output* output_ptr : outputs) {
      if (output_ptr->name == name)
        return output_ptr;
    }
    return NULL;
  }
};

#endif
