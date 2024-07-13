#ifndef CLASSES_H
#define CLASSES_H

#include <vector>
#include <String>

enum Direction{
  LEFT,
  RIGHT
};

enum Relop{
  GREATER_THAN,
  SMALLER_THAN
};

class Sensor {
public:
  String name;
  int in_port;
  bool status;

  Sensor(const String& name, int in_port) : name(name), in_port(in_port), status(false) {}
  virtual void read_sensor() = 0;
};


class Distance_sensor : public Sensor {
public:
  Distance_sensor(const String& name, int in_port) : Sensor(name, in_port) {}
  void read_sensor() override {
    // Implementation for reading distance sensor
    }
};


class Light_sensor : public Sensor {
public:
  Light_sensor(const String& name, int in_port) : Sensor(name, in_port) {}
  void read_sensor() override {
    // Implementation for reading light sensor
    }
};


class Motor {
public:
    String name;
    int in_port;
    int out_port;
    int inputRange[2];
    int outputRange[2];
    int threshold;

    Motor(const String& name, int in_port, int out_port, int input_min_val, int input_max_val, int output_min_val, int output_max_val, int threshold)
           : name(name), in_port(in_port), out_port(out_port) {
            inputRange[0] = input_min_val;
            inputRange[1] = input_max_val;
            outputRange[0] = output_min_val;
            outputRange[1] = output_max_val;
            threshold = threshold;
        }
    virtual void set_calibration() = 0;
    virtual void execute_command() = 0;

};


class motor_type_A : public Motor {
public:
    motor_type_A(const String& name, int in_port, int out_port, int input_min_val, int input_max_val, int output_min_val, int output_max_val, int threshold)
        : Motor(name, in_port, out_port, input_min_val, input_max_val, output_min_val, output_max_val, threshold) {}
    void set_calibration() override {
        // Implementation for motor type A calibration
    }
    void execute_command() override {
        // Implementation for motor type A command execution
    }
};

class End_condition{
  Sensor* sensor;
  int threshold;
  Relop relop;
  End_condition(Sensor* sensor, int threshold, Relop relop): sensor(sensor), threshold(threshold), relop(relop){}
  bool ended(){
    // TO-DO
  }

};

class Action{
public:
  Motor* motor;
  Direction direction;
  End_condition end_condition;
  Action(Motor* motor, Direction direction, End_condition end_condition): motor(motor), direction(direction), end_condition(end_condition){}

  void execute(int speed){
    // TO-DO
  }
  void halt(){
    // TO-DO: stop motor operation
  }
};

class Command{
public:
  String name;
  std::vector<Action*> actions;

  Command(const String& name, int defaule_param = -1): name(name), default_param(default_param){};

  void push_action(Action* action){
    actions.push_back(action);
  }

  void execute(int speed = 0, int angle = 0){
    for(Action action_ptr : actions){
      action->execute(speed)
    }
    bool flag = true;
    while(flag){
      for(Action action_ptr : actions){
        if(action_ptr->end_condition.ended())
          action_ptr->halt();
      }
    }




    }
  }

};


class Sequential_command : Command{
public:
  std::vector<Command*> sub_commands;

  Sequential_command(const String& name): Command(name) {};

  void push_command(Command* command){
    sub_commands.push_back(command);
  }

};


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
};

class Connection{
public:
  String protocol_type;
  String sensory_system_id;
  bool status;
  
  Connection(String protocol_type, String sensory_system_id): protocol_type(protocol_type), sensory_system_id(sensory_system_id), status(false){}

  virtual void configure_connection();

};


class Wifi : Connection{
public:
  Wifi(String protocol_type, String sensory_system_id): Connection(protocol_type, sensory_system_id){}
  void configure_connection();

};

class Bluetooth : Connection{
public:
  Bluetooth(String protocol_type, String sensory_system_id): Connection(protocol_type, sensory_system_id){}
    void configure_connection();
};

class System{
  Hand* hand;
  Connection* connection;

};



#endif







