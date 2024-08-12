#ifndef WIFI_COMMUNICATION
#define WIFI_COMMUNICATION
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include "classes.h"
#include <Base64.h>

extern Hand* hand;
extern Received_command cmd;
extern SemaphoreHandle_t xMutex_payload;
extern volatile bool is_semaphore_being_deleted;

void process_payload_and_execute_command(const uint8_t* command_payload);
void yaml_to_json(const char *yaml_str, bool recreate_resources);

#define num_of_args 2

const char* ssid = "Smart_Prosthesis";          // Name of the Wi-Fi network
const char* password = "100inIOT";      // Password for the Wi-Fi network

WebServer server(80);
String yaml_configs;  // Global variable to store the configs
const char* command;       // Global variable to store the command


/**
 * @brief Serves the HTML page for configuring the system.
 * 
 * This function serves an HTML page with a multi-line text box where users can input YAML configurations for the system.
 * It provides styling for the page and a submit button to send the configuration data.
 */
void send_configs_page() {
  // Serve the HTML page with a multi-line text box, a send button, and some styling
  server.send(200, "text/html",
    "<html>"
    "<head>"
    "<style>"
    "@import url('https://fonts.googleapis.com/css2?family=Roboto:wght@400;700&display=swap');"
    "body { font-family: 'Roboto', sans-serif; background-color: #1a1a1a; color: #e0e0e0; padding: 20px; text-align: center; }"
    "h1 { color: #00bfff; margin-bottom: 20px; }"
    "form { background: #2a2a2a; padding: 20px; border-radius: 10px; max-width: 600px; margin: 0 auto; border: 1px solid #00bfff; }"
    "textarea { width: 100%; padding: 15px; border: 1px solid #00bfff; border-radius: 5px; background: #1a1a1a; color: #e0e0e0; font-family: 'Roboto', sans-serif; font-size: 14px; }"
    "input[type=submit] { background: #00bfff; color: #1a1a1a; padding: 10px 20px; border: none; border-radius: 5px; cursor: pointer; font-size: 16px; margin-top: 20px; }"
    "input[type=submit]:hover { background: #0099cc; }"
    "</style>"
    "</head>"
    "<body>"
    "<h1>Smart Prosthesis Configurations</h1>"
    "<form action=\"/submit\" method=\"post\">"
    "Enter your configs (YAML): <br><textarea name=\"textbox\" rows=\"20\" cols=\"50\">"
    "file_type: config_system\n"
    "\n"
    "# inputs type options: {'BLE_input', 'Wifi_input'}\n"
    "inputs:\n"
    "  - name: 'leg_pressure_sensor' # string (required)\n"
    "    id: 1\n"
    "    type: 'Wifi_input' # string (required)\n"
    "    function:\n"
    "      name: 'sensor_1_func' # string (required)\n"
    "      parameters:\n"
    "        speed: 100\n"
    "        param2: 500\n"
    "        param3: 20\n"
    "\n"
    "outputs:\n"
    "  - name: 'finger1_dc' # string (required)\n"
    "    type: 'DC_motor' # string (required)\n"
    "    pins:\n"
    "      - type: 'in1_pin' # string (required)\n"
    "        pin_number: 19 # int (required)\n"
    "      - type: 'in2_pin' # string (required)\n"
    "        pin_number: 21 # int (required)\n"
    "      - type: 'sense_pin' # string (required)\n"
    "        pin_number: 34 # int (required)\n"
    "    safety_threshold: 20\n"
    "  - name: 'finger2_dc' # string (required)\n"
    "    type: 'DC_motor' # string (required)\n"
    "    pins:\n"
    "      - type: 'in1_pin' # string (required)\n"
    "        pin_number: 23 # int (required)\n"
    "      - type: 'in2_pin' # string (required)\n"
    "        pin_number: 22 # int (required)\n"
    "      - type: 'sense_pin' # string (required)\n"
    "        pin_number: 35 # int (required)\n"
    "    safety_threshold: 20\n"
    "</textarea><br><br>"
    "<input type=\"submit\" value=\"Send\">"
    "</form>"
    "</body>"
    "</html>"
  );
}

/**
 * @brief Handles the POST request to receive YAML configurations from the web page.
 * 
 * This function processes POST requests sent to the server, retrieves the YAML configurations from the request, converts them to JSON,
 * and updates the system configuration accordingly. It sends a response back to the client indicating success or failure.
 */
void get_configs_from_web() {
  if (server.method() == HTTP_POST) {
    // Get the multi-line text box content
    yaml_configs = server.arg("textbox");
    yaml_to_json(yaml_configs.c_str(), true);

    // Send response back to the client
    server.send(200, "text/html", "<html><body><h1>Text received</h1></body></html>");

  } else {
    server.send(405, "text/plain", "Method Not Allowed");
  }
}

/**
 * @brief Serves the HTML page displaying the activity of connected sensors.
 * 
 * This function generates an HTML page that lists all connected sensors and their last activity time. It provides styling for the page
 * and shows the status of each sensor.
 */
void send_sensors_page() {
  String html = "<html>"
                "<head>"
                "<style>"
                "body { font-family: Arial, sans-serif; background-color: #f0f0f0; padding: 20px; }"
                "h1 { color: #333; }"
                ".content { background: #fff; padding: 15px; border-radius: 8px; box-shadow: 0 0 10px rgba(0, 0, 0, 0.1); margin-top: 20px; }"
                ".item { margin-bottom: 10px; padding: 10px; background: #f9f9f9; border: 1px solid #ddd; border-radius: 4px; }"
                "</style>"
                "</head>"
                "<body>"
                "<h1> Sensors Activity </h1>"
                "<div class='content'>";
  
  for (Input* sensor_ptr : hand->inputs) {
    int sensor_counter = 0;
    if (sensor_ptr->last_signal_timestamp > 0){
      String sensor_summary = "Sensor " + sensor_ptr->name +" type: " + sensor_ptr->type + " last connection was " + String((millis() - sensor_ptr->last_signal_timestamp)/1000) + " seconds ago.";  
      html += "<div class='item'>" + sensor_summary + "</div>";
      sensor_counter++;
    }
    if (sensor_counter == 0){
      html += "<div class='item'>" + String("No connected sensors yet.") + "</div>";
    }
  }
  
  html += "</div>"
          "</body>"
          "</html>";

  server.send(200, "text/html", html);
}

/**
 * @brief Serves the HTML page for sending commands to the system.
 * 
 * This function serves an HTML page with input boxes where users can enter an ID and sensor value to send a command to the system.
 * It provides styling for the page and a submit button to send the command.
 */
void send_command_page() {
  // Serve the HTML page with input boxes for id and sensor_value
  server.send(200, "text/html",
    "<html>"
    "<head>"
    "<style>"
    "body { font-family: Arial, sans-serif; background-color: #ADD8E6; padding: 20px; }"
    "h1 { color: #fff; }"
    "form { background: #fff; padding: 20px; border-radius: 8px; box-shadow: 0 0 10px rgba(0,0,0,0.1); max-width: 500px; margin: auto; }"
    "input[type=number] { width: 100%; padding: 10px; border: 1px solid #ccc; border-radius: 4px; margin-bottom: 10px; }"
    "input[type=submit] { background: #4CAF50; color: white; padding: 10px 20px; border: none; border-radius: 4px; cursor: pointer; }"
    "input[type=submit]:hover { background: #45a049; }"
    "</style>"
    "</head>"
    "<body>"
    "<h1>Send Command</h1>"
    "<form action=\"/send_command\" method=\"post\">"
    "Enter ID (0-255): <br><input type=\"number\" name=\"id\" min=\"0\" max=\"255\"><br>"
    "Enter Sensor Value (0-255): <br><input type=\"number\" name=\"sensor_value\" min=\"0\" max=\"255\"><br>"
    "<input type=\"submit\" value=\"Send Command\">"
    "</form>"
    "</body>"
    "</html>"
  );
}


/**
 * @brief Handles the POST request to receive a command from the web page.
 * 
 * This function processes POST requests sent to the server, retrieves the command ID and sensor value from the request, and updates
 * the command payload. It sends a response back to the client indicating success or failure.
 */
void get_command_from_web() {
  if (server.method() == HTTP_POST) {
    if(!is_semaphore_being_deleted && xSemaphoreTake(xMutex_payload, portMAX_DELAY)){
      cmd.command_payload[0] = (uint8_t)server.arg("id").toInt();
      cmd.command_payload[1] = (uint8_t)server.arg("sensor_value").toInt();
      cmd.command_payload_len = 2;
      cmd.is_pending = true;
      xSemaphoreGive(xMutex_payload);
      // Send response back to the client
      server.send(200, "text/html", "<html><body><h1>Command received</h1></body></html>");
    } 
  else {
    server.send(405, "text/plain", "Method Not Allowed");
  }
  }
}

/**
 * @brief Handles the POST request to receive sensor data from the web page.
 * 
 * This function processes POST requests sent to the server containing comma-separated sensor values. It parses the values, updates
 * the command payload, and sends a response back to the client indicating success or failure.
 */
void get_sensor_value() {
  if (server.method() == HTTP_POST) {
    if (server.hasArg("plain")) {
      String body = server.arg("plain");

      // Split the received string by commas
      int commaIndex = 0;
      int startIndex = 0;
      int payloadLength = 0;

      // Count the number of values
      while ((commaIndex = body.indexOf(',', startIndex)) != -1) {
        payloadLength++;
        startIndex = commaIndex + 1;
      }
      // Account for the last value after the final comma
      payloadLength++;

      if (!is_semaphore_being_deleted && xSemaphoreTake(xMutex_payload, portMAX_DELAY)) {
        cmd.command_payload_len = payloadLength;
        cmd.is_pending = true;

        // Extract each byte value from the string
        startIndex = 0;
        for (int i = 0; i < payloadLength; i++) {
          int commaIndex = body.indexOf(',', startIndex);
          if (commaIndex == -1) {
            commaIndex = body.length();
          }
          String byteStr = body.substring(startIndex, commaIndex);
          cmd.command_payload[i] = (uint8_t)byteStr.toInt();
          startIndex = commaIndex + 1;
          Serial.print("cmd.command_payload[i]: ");
          Serial.println(cmd.command_payload[i]);
        }

        Serial.print("cmd.command_payload_len: ");
        Serial.println(cmd.command_payload_len);

        xSemaphoreGive(xMutex_payload);
        server.send(200, "text/plain", "Data received");
      } else {
        server.send(500, "text/plain", "Server error");
      }
    } else {
      server.send(400, "text/plain", "No data available");
    }
  } else {
    server.send(405, "text/plain", "Method Not Allowed");
  }
}

/**
 * @brief Initializes the Wi-Fi access point and sets up the web server.
 * 
 * This function sets up the Wi-Fi in Access Point mode, starts the web server, and configures the server to handle various routes
 * for serving web pages and processing requests related to configurations, commands, and sensor data.
 */
void bring_up_wifi_server() {
  Serial.println("Access Point started");
  // Set up Wi-Fi in Access Point mode
  WiFi.softAP(ssid, password);
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());

  // Set up the server to handle requests
  server.on("/", HTTP_GET, send_configs_page);
  server.on("/submit", HTTP_POST, get_configs_from_web);
  server.on("/send_command", HTTP_GET, send_command_page);
  server.on("/send_command", HTTP_POST, get_command_from_web);
  server.on("/sensors_activity", HTTP_GET, send_sensors_page);
  server.on("/sensor_data", HTTP_POST, get_sensor_value);
  server.begin();
  Serial.println("HTTP server started");
}

#endif /* WIFI_COMMUNICATION */