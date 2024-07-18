/**
  @file classes.h
  @brief This header file defines various classes used for a smart prosthesis system. 
 */

#ifndef CLASSES_H
#define CLASSES_H
#include <vector>
#include <String>



typedef void* (*FuncPtr)(void*);

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
  virtual void set_calibration() = 0
  virtual ~Input() {}
};


class Func_of_input {
  FuncPtr func_ptr;
  std::vector<double> parameters
  Func_of_input(FuncPtr func_ptr, std::vector<double> parameters) : func_ptr(func_ptr, parameters(parameters)){}

  void* execute_func(uint8_t* sensor_read_value){
    func_ptr(parameters, sensor_read_value)
  }


};

/**
 * @class Sensor
 * @brief Abstract base class for sensors.
 */
class Sensor : Input{
  public:
  int in_port;
  int out_port;
  bool status;
  Func_of_input func_of_input_obj
  Sensor(int id, const String& name, int in_port, int out_port, Func_of_input func_of_input) : Input(id, name), name(name), in_port(in_port), out_port(out_port), status(false), func_of_input_obj(func_of_input){}
  virtual void read_input() = 0;
};

class Distance_sensor : public Sensor {
  public:
  Distance_sensor(int id, const String& name, int in_port) : Sensor(id, name, in_port) {}
  void read_input() override {
      // Implementation for reading distance sensor
    }
};

class Light_sensor : public Sensor {
  public:
  Light_sensor(int id, const String& name, int in_port) : Sensor(id, name, in_port) {}
  void read_input() override {
      // Implementation for reading light sensor
    }
};

/**
 * @class Motor
 * @brief Abstract base class for motors.
 */
class Motor : Input {
public:
  int id;
  String name;
  int in_port_current;
  int out_port_spead;
  int out_port_direction;
  int inputRange[2]; 
  int outputRange[2];
  int threshold;
  Motor(int id, const String& name, int in_port, int out_port_speed, int out_port_direction, int input_min_val, int input_max_val, int output_min_val, int output_max_val, int thres)
          : Input(id, name), in_port(in_port), out_port_speed(out_port_speed), out_port_direction(out_port_direction) {
          inputRange[0] = input_min_val;
          inputRange[1] = input_max_val;
          outputRange[0] = output_min_val;
          outputRange[1] = output_max_val;
          threshold = thres;
      }
  /**
    * @brief Pure virtual function to set calibration for the motor.
    * (e.g. linear manipulation performed on the input before sent to the motor)
    */
  virtual void set_calibration() = 0;
  virtual void execute_action(int direction, int speed) = 0;
  virtual void read_input() = 0;
};

class motor_type_A : public Motor {
  public:
    motor_type_A(const String& name, int in_port, int out_port_speed, int out_port_direction, int input_min_val, int input_max_val, int output_min_val, int output_max_val, int threshold)
        : Motor(name, in_port, out_port_speed, out_port_direction, input_min_val, input_max_val, output_min_val, output_max_val, threshold) {}
    void set_calibration() override {
          // Implementation for motor type A calibration
    }
    void execute_action(int direction, int speed) override {
          // Implementation for motor type A command execution
    }
};

/**
 * @class End_condition
 * @brief Class for defining end conditions for actions. 
 * e.g. stop the action X(type Action) when the input from sensor Y(type Input) is higher than Z(type int)
 */
class End_condition{
    Input* sensor;
  int threshold;
  Relop relop;
  End_condition(Sensor* sensor, int threshold, Relop relop): sensor(sensor), threshold(threshold), relop(relop){}
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
  End_condition motor_end_condition; 
  Action(Motor* motor, Direction direction, End_condition end_condition): motor(motor), direction(direction), end_condition(end_condition){}
  /**
  * @brief Initiates the motor operation.
  */
  void execute(){
      // TO-DO
  }
  /**
  * @brief Halts the motor operation.
  */
  void halt(){
      // TO-DO: stop motor operation
  }
  /**
  * @brief check if the action's end condiiton is met or motor reached its threshold.
  */
  bool check_end_conditions(){
      return (end_condition.condition_is_met() || motor->threshold > motor.get_input())
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
  void execute(){
      for(Action action_ptr : actions){
        action->execute(speed)
    }
    bool flag = false;
    while(flag){
        flag = false;
      for(Action action_ptr : actions){
          if(action_ptr->check_end_conditions)
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
class Sequential_command : Command{
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
      for(Command* command_ptr : commands){
        command.execute();
    }
};

/**
 * @class Hand
 * @brief Represents a hand with multiple motors, sensors, and commands.
 */
class Hand{
  public:
  std::vector<Motor*> motors;
  std::vector<Sensor*> sensors;
  std::vector<Command*> commands;
  Hand(){}
  void add_motor(Motor* motor){
      motors.push_back(motor);
  }
  void add_sensor(Sensor* sensor){
      sensors.push_back(sensor);
  }
  void add_command(Command* command){
      commands.push_back(command);
  }
  //getters + setters? to every component by its name.
};

/**
 * @class Connection
 * @brief Represents a connection for a sensory system.
 */
class Connection{
  public:
  String sensory_system_id;
  bool status;

  Connection(String sensory_system_id): sensory_system_id(sensory_system_id), status(false){}
  virtual void configure_connection();
};

/**
 * @class Wifi
 * @brief Represents a Wifi connection for a sensory system.
 */
class Wifi : Connection{
  public:
  Wifi(String protocol_type, String sensory_system_id): Connection(protocol_type, sensory_system_id){}
  void configure_connection();
};

/**
 * @class Bluetooth
 * @brief Represents a Bluetooth connection for a sensory system.
 */
class Bluetooth : Connection{
  public:
  Bluetooth(String protocol_type, String sensory_system_id): Connection(protocol_type, sensory_system_id){}
    void configure_connection();
};

/**
 * @class System
 * @brief Represents the overall system with a hand and a connection.
 */
class System{
  public:
  Hand* hand;
  Connection* connection;
  System(Hand* hand, Connection* connection)
    : hand(hand), connection(connection) {}
};

#endif
