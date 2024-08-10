################
Smart Prosthesis
################

**The project simplifies the customization and configuration of smart 
prosthetic hands for diverse user’s needs. Provides a platform that enables 
the customization of smart prosthetic hands.**

overview
########

System flow
===========

The flow can be divided into two main parts: configuration and operation.

.. figure:: ./images/system_flow.png
   :width: 30%

Configuration
*************

.. figure:: ./images/configuration_diagram.png
   :width: 30%

The configuration of the hand consists of two main parts:

#. The system either loads from memory or receives from the user a YAML file
    that specifies the system components. This allows for full flexibility in modifying the hand structure in the future.
    The YAML file also associates sensors with their target functionalities (see `functionality`_).

    After receiving a new configuration file from the user, it will be saved to memory and loaded
    the next time the controller is launched.

    Example for such YAML file:

    .. _YAML_FIGURE:
   
    .. figure:: ./images/YAML.jpeg
       :width: 30%

    .. _functionality:

#. Functionality - 
    Each sensor corresponds to a specific function that defines the desired behavior of 
    the hand based on the values sensed by the sensor.

    These functions are pre-written by the administrator and can be easily added or modified as needed.
    Each sensor's functionality should be specified in the YAML file.


**note** The configuration of the hand is a one-time operation and does not need to be repeated each time the hand is relaunched.


Operation
*********

.. figure:: ./images/operation_diagram.png
   :width: 40%


After the hand is configured, the system operates with three main flows, managed by three threads:

#. BLE + WIFI listenner:
    This thread listens for asynchronous interrupts. once caught an interrupt - creates a new command object which allows the communication with the third thread.
    A new command is labeled with a "pending" flag to be executed by the third thread (described below). 
#. HW management:
    Commands received from sensors are translated into basic motor actions and stored as the current "state." Each state has an "end condition" (i.e., the current
    value sensed by the motor). The HW Management thread iterates through all motors, checks their states, and directs the motors to start an action (with custom 
    direction and speed), stop an action, or make no change to their operation.
#. Process payload and manage logic:
    This thread is responsible for parsing payloads received from sensors and triggering the relevant hand functions. These functions update the motor states used 
    for communication with the HW Management thread, which controls the motors.

.. figure:: ./images/threads_communication.png
   :width: 100%


Admin (user) Instructions
#########################

This section covers:
 #. Writing a New Configuration File (YAML): YAML Structure_
 #. Sending a New Configuration (YAML) to the Hand: Sending Configs_
 #. Writing New Hand Functions: Write Hand Function_
 #. Getting Data About Connected Sensors: Connected Sensors Data_

.. _YAML sctucture:

#. YAML sctucture
    The YAML file should start with a file type field set to config_system. The YAML consists of two main parts:
     #. Inputs (Sensors): Attributes include name, id, type, and function (the corresponding hand function pre-coded by the admin).
     #. Outputs (Motors): Attributes include name, type, and pins.
   An example YAML file is provided in this repository under the name `config_system_usecase.yaml` (or see `YAML_FIGURE`_ above).

   .. _sending configs:

#. Sending Configs
    When the hand (controller) is powered on, it acts as a Wi-Fi access point, creating a small LAN for communication. To send your YAML file:
      #. Connect to the hand's Wi-Fi network from your PC. Look for "smart_prosthesis" and enter the password: "100inIOT".
      #. Open a web browser and navigate to "192.168.4.1" (the default IP address of the controller).
      #. A text box will appear; paste your YAML file or modify the default configuration and click 'Send'.

   .. _write hand function:

#. Write A Hand Function
    Hand functions are defined and implemented in ./main/hand_functions.ino.
    The API for accessing hand elements (motors, sensors, etc.) can be found in ./main/classes.h.
    Example for accessing a parameter defined in the YAML:

    To access `param1: 100` (from the YAML file), use `params["param1"]` in your function. For more details, see the examples in `./main/hand_functions.ino`.
    (For more info see the examples in './main/hand_functions.ino')

    **note: After adding a new function, add it to the `func_map` defined in `./main/hand_functions.ino`.**

    .. _connected sensors data:

#. Connected Sensors Data
    Connect to the hand's Wi-Fi network as described above and access the page at 192.168.4.1/sensors_summary.
    
    


   






.. sectnum::
   :start: 1










