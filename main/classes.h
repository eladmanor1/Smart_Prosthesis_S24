/**
  @file classes.h
  @brief This header file defines various classes used for a smart prosthesis system. 
 */

#ifndef CLASSES_H
#define CLASSES_H
#include <vector>
#include <String>

typedef void (*FuncPtr)(std::vector<double>, const uint8_t*);
#define OUTPUT_MIN_VAL 10
#define OUTPUT_MAX_VAL 100

/**
 * @enum Direction
 * @brief Enumerates the possible directions for motor movement.
 */
enum Direction{
  LEFT,
  RIGHT
};


/**
 * @enum Relop
 * @brief Enumerates the possible relational operations for (*)end conditions. (* - will be explained later)
 */
enum Relop{
  GREATER_THAN,
  SMALLER_THAN
};

/**
 * @class Sensor
 * @brief Abstract base class for system elements that can serve as inputs. (motors / sensors)
 */
class Input {
public:
  int id;
  String name;
  Input(int id, String name): id(id), name(name){}
  virtual void debug_print(){
    Serial.println("--INPUT--");
    Serial.print("id : ");
    Serial.println(id);
    Serial.print("name : ");
    Serial.println(name);
    }
  virtual ~Input() {}
};

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


class Func_of_input {
public:
  FuncPtr func_ptr;
  std::vector<double> parameters;
  Func_of_input(FuncPtr func_ptr, std::vector<double> parameters) : func_ptr(func_ptr), parameters(parameters){}

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
  Sensor(int id, const String& name, Func_of_input func_of_input, int in_pin=-1, int out_pin=-1) : Input(id, name), func_of_input_obj(func_of_input), in_pin(in_pin), out_pin(out_pin), status(false){}
  void debug_print() override{
    Serial.println("--Sensor--");
    Serial.print("id : ");
    Serial.println(id);
    Serial.print("name : ");
    Serial.println(name);
    Serial.print("in_pin : ");
    Serial.println(in_pin);
    Serial.print("out_pin : ");
    Serial.println(out_pin);
     }
  virtual void read_input(){}
};

class Distance_sensor : public Sensor {
  public:
  Distance_sensor(int id, const String& name, int in_pin, int out_pin, Func_of_input func_of_input) : Sensor(id, name, func_of_input, in_pin, out_pin) {}
  void read_input() override {
      // Implementation for reading distance sensor
    }
};

class Light_sensor : public Sensor {
  public:
  Light_sensor(int id, const String& name, int in_pin, int out_pin, Func_of_input func_of_input) : Sensor(id, name, func_of_input, in_pin, out_pin) {}
  void read_input() override {
      // Implementation for reading light sensor
    }
};

/**
 * @class Motor
 * @brief Abstract base class for motors.
 */
class Motor : public Output {
public:
  int outputRange[2]; 
  Motor(String name, String type, int output_min_val, int output_max_val)
          : Output(name, type){
          outputRange[0] = output_min_val;
          outputRange[1] = output_max_val;
      }
  /**
    * @brief Pure virtual function to set calibration for the motor.
    * (e.g. linear manipulation performed on the input before sent to the motor)
    */
  virtual void set_calibration(){}
  virtual void execute_action(int direction, int speed){}
  virtual int read_input(){}
};

class Servo_motor : public Motor {
public:
  int control_pin;
  Servo_motor(String name, String type, int control_pin)
      : Motor(name, type, OUTPUT_MIN_VAL, OUTPUT_MAX_VAL), control_pin(control_pin){}
  void set_calibration() override {
        // Implementation for motor type A calibration
  }
  void debug_print() override{
    Serial.println("--Motor--");
    Serial.print("name : ");
    Serial.println(name);
    Serial.print("type : ");
    Serial.println(type);
    Serial.print("control_pin : ");
    Serial.println(control_pin);
     }
  void execute_action(int direction, int speed) override {
        // Implementation for motor type A command execution
  }
  int read_input() override {
        // Implementation for motor type A command execution
  }
};

class DC_motor : public Motor {
public:
  int in1_pin;
  int in2_pin;
  int sense_pin;
  int threshold;
  DC_motor(String name, String type,  int in1_pin ,  int in2_pin,  int sense_pin , int threshold)
      : Motor(name, type, OUTPUT_MIN_VAL, OUTPUT_MAX_VAL), in1_pin(in1_pin) , in2_pin(in2_pin) , sense_pin(sense_pin) ,threshold(threshold){}
  void set_calibration() override {
        // Implementation for motor type A calibration
  }
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
    Serial.print("threshold : ");
    Serial.println(threshold);
    }
  void execute_action(int direction, int speed) override {
        // Implementation for motor type A command execution
  }
  int read_input() override {
        // Implementation for motor type A command execution
  }
};



/**
 * @class End_condition
 * @brief Class for defining end conditions for actions. 
 * e.g. stop the action X(type Action) when the input from sensor Y(type Input) is higher than Z(type int)
 */
class End_condition{
public:
  Input* sensor;
  int threshold;
  Relop relop;
  End_condition(Input* sensor, int threshold, Relop relop): sensor(sensor), threshold(threshold), relop(relop){}
  /**
     * @brief Checks if the end condition is met.
     * @return True if the end condition is met, false otherwise.
  */
  bool condition_is_met(){
      // TO-DO
  }
};

/**
 * @class Action
 * @brief Class for defining actions involving motors and end conditions. 
 * e.g. Action X includes a motor and relevant parameters to the motor, and an End_condition object 
 *      defines when to stop the action after it was triggered.
 * @note An action can be stopped also due to motor's threshold, even before action's enc_condition is met. 
 */
class Action{
public:
  Motor* motor;
  int speed;
  Direction direction;
  End_condition end_condition; 
  Action(Motor* motor, Direction direction, End_condition end_condition): motor(motor), direction(direction), end_condition(end_condition){}
  /**
  * @brief Initiates the motor operation.
  */
  void execute(int speed){
      // TO-DO
  }
  /**
  * @brief Halts the motor operation.
  */
  void halt(){
      // TO-DO: stop motor operation
  }
  /**
  * @brief check if the action's end condiiton is met *****what about threshold.
  */
  bool check_end_conditions(){
      return (end_condition.condition_is_met());
  }
};

/**
 * @class Command
 * @brief Class for defining commands composed of multiple actions that is executed simultaneously.
 */
class Command{
  public:
  String name;
  std::vector<Action*> actions;
  Command(const String& name): name(name){};
  
  void push_action(Action* action){
      actions.push_back(action);
  }
  void execute(int speed){
      for(Action* action_ptr : actions){
        action_ptr->execute(speed);
    }
    bool flag = false;
    while(flag){
        flag = false;
      for(Action* action_ptr : actions){
        if(action_ptr->check_end_conditions())
          action_ptr->halt();
        else flag = true;
      }
    }
  }
};

/**
 * @class Sequential_command
 * @brief Class for defining sequential commands composed of multiple sub-commands that occurs synchronously.
 */
class Sequential_command : public Command{
  public:
  std::vector<Command*> sub_commands;
  Sequential_command(const String& name): Command(name) {};
  
  void push_command(Command* command){
      sub_commands.push_back(command);
  }
  /**
  * @brief execute all sub_commands synchronously.
  */
  void execute(int speed = -1){
      for(Command* command_ptr : sub_commands){
        command_ptr->execute(speed);
    }
  }
};

/**
 * @class Hand
 * @brief Represents a hand with multiple motors, sensors, and commands.
 */
class Hand{
public:
  std::vector<Output*> outputs;
  std::vector<Input*> inputs;
  std::vector<Command*> commands;
  Hand(){}
  void add_output(Output* output){
      outputs.push_back(output);
  }
  void add_input(Input* input){
      inputs.push_back(input);
  }
  void add_command(Command* command){
      commands.push_back(command);
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

// /**
//  * @class Connection
//  * @brief Represents a connection for a sensory system.
//  */
// class Connection{
// public:
//   String sensory_system_id;
//   bool status;

//   Connection(String sensory_system_id): sensory_system_id(sensory_system_id), status(false){}
//   virtual void configure_connection();
// };

// /**
//  * @class Wifi
//  * @brief Represents a Wifi connection for a sensory system.
//  */
// class Wifi : Connection{
// public:
//   Wifi(String protocol_type, String sensory_system_id): Connection(protocol_type, sensory_system_id){}
//   void configure_connection();
// };

// /**
//  * @class Bluetooth
//  * @brief Represents a Bluetooth connection for a sensory system.
//  */
// class Bluetooth : Connection{
//   public:
//   Bluetooth(String protocol_type, String sensory_system_id): Connection(protocol_type, sensory_system_id){}
//     void configure_connection();
// };

/**
 * @class System
 * @brief Represents the overall system with a hand.
 */
class System{
  public:
  Hand* hand;
  System(Hand* hand)
    : hand(hand){}
};

#endif
