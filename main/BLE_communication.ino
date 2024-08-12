#ifndef BLE_COMMUNICATION
#define BLE_COMMUNICATION
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <classes.h>

extern SemaphoreHandle_t xMutex_payload;
extern Received_command cmd;
extern volatile bool is_semaphore_being_deleted;
// -------------------------------------------------------------------------------------------------- //
// ------------------------------------------ SYSTEM CONSTS  ---------------------------------------- //
// -------------------------------------------------------------------------------------------------- //

//  ------------- UUIDs  -----------  //
#define SENSOR_SERVICE_UUID "e0198002-7544-42c1-0000-b24344b6aa70"
#define SENSOR_ON_WRITE_CHARACTERISTIC_UUID "e0198002-7544-42c1-0001-b24344b6aa70"

bool deviceConnected = false;

// ---------------------------------------------------------------------------------------------------------- //
// ------------------------------------------ CALLBACKS DEFENITIONS  ---------------------------------------- //
// ---------------------------------------------------------------------------------------------------------- //
// Server Callbacks
class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer *pServer) {
      deviceConnected = true;
    Serial.println("BLE client Connected");
  };

  void onDisconnect(BLEServer *pServer) {
        deviceConnected = false;
      Serial.println("BLE client Disconnected");
      pServer->startAdvertising();  // Restart advertising
    }
  };

    class SensorCallbacks : public BLECharacteristicCallbacks {
        void onWrite(BLECharacteristic *pCharacteristic) {  //execute one movement from the 'live control' or from the 'movement editing'
          if(!is_semaphore_being_deleted && xSemaphoreTake(xMutex_payload, portMAX_DELAY)){
            cmd.command_payload_len = pCharacteristic->getLength();
            Serial.print("Payload size: ");
            Serial.println(cmd.command_payload_len);
            Serial.print("Payload data: ");
            const uint8_t* data_ptr = pCharacteristic->getData();
            for (size_t i = 0; i < cmd.command_payload_len; ++i) {
              cmd.command_payload[i] = data_ptr[i];
              Serial.print(cmd.command_payload[i], HEX);
              Serial.print(" ");
            }
            Serial.println();
            cmd.is_pending = true;
            xSemaphoreGive(xMutex_payload);
          }
      };
    };
// ---------------------------------------------------------------------------------------------------------- //
// ---------------------------------------------------------------------------------------------------------- //
// ---------------------------------------------------------------------------------------------------------- //


// ---------------------- Sensor Service Setup ------------------------ //


/**
 * @brief Sets up the BLE sensor service and its characteristics.
 * 
 * This function initializes a BLE service for sensors and creates a writable characteristic for receiving sensor commands.
 * The characteristic's callback is set to handle write operations, which will invoke `SensorCallbacks` for processing payloads.
 * 
 * @param pServer Pointer to the BLE server instance.
 */
void sensorServiceSetup(BLEServer *pServer) {
  BLEService *pSensorService = pServer->createService(SENSOR_SERVICE_UUID);
  BLECharacteristic *pSensorOnWriteCharacteristic = pSensorService->createCharacteristic(
    SENSOR_ON_WRITE_CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_WRITE);
  pSensorOnWriteCharacteristic->setCallbacks(new SensorCallbacks());
  pSensorService->start();
}


/**
 * @brief Configures BLE services on the BLE server.
 * 
 * This function sets up the necessary services for the BLE server by calling `sensorServiceSetup`. It ensures that all required
 * services are created and ready to handle BLE communication.
 * 
 * @param pServer Pointer to the BLE server instance.
 */
void setServices(BLEServer *pServer) {
  sensorServiceSetup(pServer);
}


/**
 * @brief Configures and starts BLE advertising.
 * 
 * This function sets up the BLE advertising parameters and starts advertising the BLE service. It includes setting intervals,
 * preferred parameters, and adding service UUIDs to the advertising packet. It also handles scan response configuration.
 * 
 * @param pServer Pointer to the BLE server instance.
 */
void setAdvertizing(BLEServer *pServer) {
  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->setMinInterval(100);
  pAdvertising->setMaxInterval(200);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  pAdvertising->addServiceUUID(SENSOR_SERVICE_UUID);
  pAdvertising->start();
  pAdvertising->setScanResponse(true);
  Serial.println("BLE advertising started!");
}


/**
 * @brief Initializes the BLE stack, sets up services, and starts advertising.
 * 
 * This function initializes the BLE stack with a specific device name, creates a BLE server, sets up services and advertising,
 * and configures a GPIO pin for LED output. It prepares the BLE communication for use in the smart prosthesis system.
 */
void init_BLE() {
  BLEDevice::init("SMART_PROSTHESIS");

  // Create the BLE Server
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create services for BLE Server (battery , config , sensor , info)
  setServices(pServer);

  // Start advertising
  setAdvertizing(pServer);

  // Set GPIO2 as output (for an LED)
  pinMode(2, OUTPUT);
}
#endif /* BLE_COMMUNICATION */
