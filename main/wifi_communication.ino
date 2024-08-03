#ifndef WIFI_COMMUNICATION
#define WIFI_COMMUNICATION
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include "classes.h"

extern Hand* hand;
void process_payload_and_execute_command(const uint8_t* command_payload);
void yaml_to_json(const char *yaml_str);

#define num_of_args 2

const char* ssid = "Smart_Prosthesis_c_elad";          // Name of the Wi-Fi network
const char* password = "your_PASSWORD";      // Password for the Wi-Fi network

WebServer server(80);
String yaml_configs;  // Global variable to store the configs
const char* command;       // Global variable to store the command

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
    "# file type options: {'config_system','debug_mode'}\n"
    "file_type: config_system\n"
    "\n"
    "# inputs type options: {'BLE_input', 'analog_input', 'digital_input'}\n"
    "# transform type options: {'linear'}\n"
    "inputs:\n"
    "  - name: 'leg_pressure_sensor' # string (required)\n"
    "    id: 1\n"
    "    type: 'BLE_input' # string (required)\n"
    "    function:\n"
    "      name: 'sensor_1_func' # string (required)\n"
    "      parameters:\n"
    "        speed: 100\n"
    "        param2: 500\n"
    "        param3: 20\n"
    "\n"
    "outputs:\n"
    "  - name: 'finger1_servo' # string (required)\n"
    "    type: 'servo_motor' # string (required)\n"
    "    pins:\n"
    "      - type: 'control' # string (required)\n"
    "        pin_number: 14 # int (required)\n"
    "\n"
    "  - name: 'finger1_dc' # string (required)\n"
    "    type: 'DC_motor' # string (required)\n"
    "    pins:\n"
    "      - type: 'in1_pin' # string (required)\n"
    "        pin_number: 19 # int (required)\n"
    "      - type: 'in2_pin' # string (required)\n"
    "        pin_number: 21 # int (required)\n"
    "      - type: 'sense_pin' # string (required)\n"
    "        pin_number: 34 # int (required)\n"
    "    threshold: 20\n"
    "</textarea><br><br>"
    "<input type=\"submit\" value=\"Send\">"
    "</form>"
    "</body>"
    "</html>"
  );
}


void get_configs() {
  if (server.method() == HTTP_POST) {
    // Get the multi-line text box content
    yaml_configs = server.arg("textbox");
    yaml_to_json(yaml_configs.c_str());
    // Send response back to the client
    server.send(200, "text/html", "<html><body><h1>Text received</h1></body></html>");

  } else {
    server.send(405, "text/plain", "Method Not Allowed");
  }
}


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
                "<h1>Array Content Display</h1>"
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

void get_command() {
  if (server.method() == HTTP_POST) {
    // Get the command text box content
    uint8_t command_payload[2];
    command_payload[0] = (uint8_t)server.arg("id").toInt();
    command_payload[1] = (uint8_t)server.arg("sensor_value").toInt();
    process_payload_and_execute_command(command_payload);
    // Send response back to the client
    server.send(200, "text/html", "<html><body><h1>Command received</h1></body></html>");
  } else {
    server.send(405, "text/plain", "Method Not Allowed");
  }
}

void bring_up_wifi_server() {
  Serial.println("Access Point started");
  // Set up Wi-Fi in Access Point mode
  WiFi.softAP(ssid, password);
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());

  // Set up the server to handle requests
  server.on("/", HTTP_GET, send_configs_page);
  server.on("/submit", HTTP_POST, get_configs);
  server.on("/send_command", HTTP_GET, send_command_page);
  server.on("/send_command", HTTP_POST, get_command);
  server.on("/sensors_summary", HTTP_GET, send_sensors_page);
  server.begin();
  Serial.println("HTTP server started");
}

#endif /* WIFI_COMMUNICATION */