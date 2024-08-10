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
   :width: 60%

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
   :width: 60%


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
   :width: 80%


Admin (user) Instructions
#########################

This section will describe:
 #. How to write a new valid configuration file (YAML file). `YAML sctucture`_
 #. How to send new configuration (YAML file) to the hand. `sending configs`_
 #. How to wtire new 'hand functions' (the hand behaviour that later assiciated with a sensor) `write hand function`_
 #. How to get data about connected sensors `connected sensors data`_

.. _YAML sctucture:

#. YAML sctucture
    The YAML should start with 'file type' field with value config_system. The YAML consists of 2 main parts:
     #. Inputs (sensors) - has attributes: name, id, type and function (the function is the corrisponding hand function that was pre coded by the admin)
     #. Outputs (motors) - has attributes: name, type, pins
   example to such yaml file is added to this repo, under the name 'config_system_usecase.yaml. (or above: `YAML_FIGURE`_)

   .. _sending configs:

#. sending configs
    After the hand (controller) will be powered on, it will serve as wifi 'hot spot', creating a small local LAN for communication with the hand.
    To send your YAML to the hand, you should
      #. connect to the hand wifi from you pc - search for "smart_prosthesis", enter password: "100inIOT"
      #. open chrome and insert "192.168.4.1" (this is the controller default IP address)
      #. a new text box will appear, paste your YAML to this textx box and click 'send'

   .. _write hand function:

#. write hand function
    The hand functions are defined and implemented in './main/hand_functions.ino'.
    The API for accessing the hand elements (motors/sensors etc) can be found in './main/classes.h'
    accessing the parameters of a function (which defined in the YAML)

    For example: param1: 100 (from the YAML file) write in your fuction: params["param1"]
    (example for such functions are in './main/hand_functions.ino')

    After adding a new function, add it to the map: 'func_map' defined in './main/hand_functions.ino'

    .. _connected sensors data:

#. Connected sensors data
    Connect to the hand wifi as described above, and access the page: '192.168.4.1/sensors_summary'
    
    


   






.. sectnum::
   :start: 1










