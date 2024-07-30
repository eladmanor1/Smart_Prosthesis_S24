#ifndef WIFI_COMMUNICATION
#define WIFI_COMMUNICATION
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

void process_payload_and_execute_command(const uint8_t* command_payload);
void yaml_to_json(const char *yaml_str);

#define num_of_args 2

const char* ssid = "Smart_Prosthesis_c";          // Name of the Wi-Fi network
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
    "body { font-family: 'Roboto', sans-serif; background-color: #0D0D0D; color: #E0E0E0; padding: 20px; }"
    "h1 { color: #00BFFF; text-align: center; margin-bottom: 20px; text-shadow: 0 0 10px rgba(0, 191, 255, 0.7); }"
    "form { background: #1A1A1A; padding: 20px; border-radius: 10px; box-shadow: 0 0 20px rgba(0, 191, 255, 0.5); max-width: 600px; margin: auto; border: 1px solid #00BFFF; }"
    "textarea { width: 100%; padding: 15px; border: 1px solid #00BFFF; border-radius: 8px; background: #0D0D0D; color: #E0E0E0; font-family: 'Roboto', sans-serif; font-size: 14px; box-shadow: inset 0 0 10px rgba(0, 191, 255, 0.5); }"
    "input[type=submit] { background: #00BFFF; color: #0D0D0D; padding: 12px 25px; border: none; border-radius: 8px; cursor: pointer; font-size: 16px; transition: background 0.3s ease, box-shadow 0.3s ease; display: block; margin: 20px auto 0; box-shadow: 0 0 10px rgba(0, 191, 255, 0.5); }"
    "input[type=submit]:hover { background: #1E90FF; box-shadow: 0 0 20px rgba(30, 144, 255, 0.7); }"
    "</style>"
    "</head>"
    "<body>"
    "<h1>Smart Prosthesis Configurations</h1>"
    "<form action=\"/submit\" method=\"post\">"
    "Enter your configs (YAML): <br><textarea name=\"textbox\" rows=\"20\" cols=\"50\"># file type options: {'config_system','debug_mode'}\n"
    "file_type: config_system\n"
    "\n"
    "# inputs type options: {'BLE_input', 'analog_input', 'digital_input'}\n"
    "# transform type options: {'linear'}\n"
    "inputs:\n"
    "  - name: 'leg_pressure_sensor' # string (required)\n"
    "    type: 'BLE_input' # string (required)\n"
    "    function:\n"
    "      name: \"general_leg_pressure_func1\" # string (required)\n"
    "      parameters:\n"
    "        - 100\n"
    "        - 500\n"
    "        - 20\n"
    "\n"
    "  - name: 'button' # string (required)\n"
    "    type: 'digital_input' # string (required for analog/digital input only)\n"
    "    pin: 2 # int (required for analog/digital input only)\n"
    "    function:\n"
    "      name: \"general_button_func1\" # string (required)\n"
    "      parameters:\n"
    "        - 7\n"
    "\n"
    "# --- do we want to let the admin define itself the \"motors\"(output receivers)? , in this way we support general system,\n"
    "# --- output receiver types: for example: {'DC_motor','servo_motor' ,'stepper_motor , 'led','RGB'} , but actually it can be anything, defined by the user's need\n"
    "outputs:\n"
    "  - name: 'finger_1' # string (required)\n"
    "    type: 'DC_motor' # string (required)\n"
    "    pins:\n"
    "      - type: 'speed' # string (required)\n"
    "        pin_number: 4 # int (required)\n"
    "      - type: 'direction' # string (required)\n"
    "        pin_number: 5 # int (required)\n"
    "      - type: 'current' # string (required)\n"
    "        pin_number: 12 # int (required)\n"
    "\n"
    "  - name: 'finger_2' # string (required)\n"
    "    type: 'DC_motor' # string (required)\n"
    "    pins:\n"
    "      - type: 'speed' # string (required)\n"
    "        pin_number: 6 # int (required)\n"
    "      - type: 'direction' # string (required)\n"
    "        pin_number: 7 # int (required)\n"
    "      - type: 'current' # string (required)\n"
    "        pin_number: 13 # int (required)\n"
    "\n"
    "  - name: 'wrist' # string (required)\n"
    "    type: 'servo_motor' # string (required)\n"
    "    pins:\n"
    "      - type: 'speed' # string (required)\n"
    "        pin_number: 8 # int (required)\n"
    "      - type: 'direction' # string (required)\n"
    "        pin_number: 9 # int (required)\n"
    "      - type: 'current' # string (required)\n"
    "        pin_number: 14 # int (required)\n"
    "\n"
    "  - name: 'led_1' # string (required)\n"
    "    type: 'led'\n"
    "    usage: 'power_indicator'\n"
    "    pins:\n"
    "      - type: 'red_light' # string (required)\n"
    "        pin_number: 2 # int (required)\n"
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
  server.begin();
  Serial.println("HTTP server started");
}

#endif /* WIFI_COMMUNICATION */