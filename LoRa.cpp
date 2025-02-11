#include <Arduino.h>
#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include <ArduinoJson.h>  // JSON library for creating payload

// New LoRaWAN keys
uint8_t devEui[] = { 0x70, 0xB3, 0xD5, 0x7E, 0xD0, 0x06, 0xB7, 0xE2 }; 
uint8_t appEui[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04 }; 
uint8_t appKey[] = { 0xD6, 0x77, 0xF7, 0xDF, 0x4A, 0x59, 0xC2, 0x13, 0xFE, 0xE2, 0x54, 0x19, 0x89, 0x68, 0x1F, 0x41 }; 

// Pin mapping for Heltec WiFi LoRa 32 V3
const lmic_pinmap lmic_pins = {
    .nss = 5,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = 23,
    .dio = { 26, 33, LMIC_UNUSED_PIN }
};

// Variables for storing parsed data
String state = "";
String date = "";
float latitude = 0.0;
float longitude = 0.0;

unsigned long lastSendTime = 0;
unsigned long lastLocationSendTime = 0;
bool isLocked = false;
float lastKnownLatitude = 0.0;
float lastKnownLongitude = 0.0;

char incomingData[256];
int dataIndex = 0;

// Declare appData and appDataSize
char appData[256];  // Buffer to hold the LoRaWAN payload
int appDataSize = 0;  // Size of the payload

// DeviceState enum
enum DeviceState_t {
    DEVICE_STATE_INIT,
    DEVICE_STATE_JOIN,
    DEVICE_STATE_SEND,
    DEVICE_STATE_CYCLE,
    DEVICE_STATE_SLEEP
};

DeviceState_t deviceState = DEVICE_STATE_INIT;

// LoRaWAN related functions
void handleReceivedData();
void prepareTxFrame(uint8_t port);
void parseNfcMessage(char *message);
void sendLocationUpdate();
void sendAlertMessage();

// Define LMIC pin mapping and get keys for LoRaWAN
void os_getDevEui(uint8_t *buf) {
    memcpy(buf, devEui, 8);
}

void os_getAppEui(uint8_t *buf) {
    memcpy(buf, appEui, 8);
}

void os_getDevKey(uint8_t *buf) {
    memcpy(buf, appKey, 16);
}

// Add missing function os_getArtEui
void os_getArtEui(uint8_t *buf) {
    memcpy(buf, appEui, 8);  // Use the same appEui for ArtEui
}

void setup() {
    Serial.begin(115200);  
    Serial1.begin(9600);   
    Serial.println("LoRaWAN and UART Initialized");

    // Initialize LMIC
    os_init();
    LMIC_reset();

    deviceState = DEVICE_STATE_INIT;
}

void loop() {
    while (Serial1.available() > 0) {
        char receivedByte = Serial1.read(); 
        if (receivedByte == '\n') {         
            handleReceivedData();           
            dataIndex = 0;                 
        } else {
            if (dataIndex < sizeof(incomingData) - 1) { 
                incomingData[dataIndex++] = receivedByte;
            }
        }
    }

    // Send location update every 15 minutes if bike is locked
    unsigned long currentMillis = millis();
    if (isLocked && currentMillis - lastLocationSendTime >= 900000) { // 15 minutes
        sendLocationUpdate();
        lastLocationSendTime = currentMillis;
    }

    // Check for alert condition: location change after locking
    if (isLocked && (latitude != lastKnownLatitude || longitude != lastKnownLongitude)) {
        sendAlertMessage();
        lastKnownLatitude = latitude;
        lastKnownLongitude = longitude;
    }

    switch (deviceState) {
        case DEVICE_STATE_INIT:
            LMIC_reset(); // Ensure LMIC is reset before joining
            deviceState = DEVICE_STATE_JOIN;
            break;
        case DEVICE_STATE_JOIN:
            Serial.println("Joining LoRaWAN network...");
            LMIC_startJoining();
            break;
        case DEVICE_STATE_SEND:
            prepareTxFrame(1); 
            Serial.println("Sending data over LoRaWAN...");
            LMIC_setTxData2(1, (uint8_t*)appData, appDataSize, 0);
            deviceState = DEVICE_STATE_CYCLE;
            break;
        case DEVICE_STATE_CYCLE:
            Serial.println("Waiting for the next transmission cycle...");
            os_runloop_once(); // Handle LMIC events
            deviceState = DEVICE_STATE_SLEEP;
            break;
        case DEVICE_STATE_SLEEP:
            Serial.println("Going to sleep...");
            os_runloop_once(); // Sleep LMIC
            break;
        default:
            deviceState = DEVICE_STATE_INIT;
            break;
    }
}

void handleReceivedData() {
    Serial.println("Received data from NFC device:");
    Serial.println(incomingData);

    // Parse the received NFC message
    parseNfcMessage(incomingData);

    // Prepare the LoRaWAN payload with the parsed data
    String jsonMessage = "{\"date\":\"" + date + "\", \"state\":\"" + state + 
                         "\", \"latitude\":" + String(latitude, 6) + 
                         ", \"longitude\":" + String(longitude, 6) + "}";
    
    // Copy the JSON message into the appData buffer for transmission
    strncpy((char*)appData, jsonMessage.c_str(), sizeof(appData));
    appDataSize = jsonMessage.length();

    // Trigger LoRaWAN transmission
    deviceState = DEVICE_STATE_SEND;
}

// Function to parse the NFC message
void parseNfcMessage(char *message) {
    // Example of expected NFC message format:
    // DATE: 02/12/2024 14:30:01, STATE: LOCKED, LOCATION: 42.089654, -75.970346
    String msg = String(message);
    int dateIndex = msg.indexOf("DATE: ") + 6;
    int stateIndex = msg.indexOf("STATE: ") + 7;
    int locationIndex = msg.indexOf("LOCATION: ") + 10;

    // Extract DATE, STATE, and LOCATION from the message
    date = msg.substring(dateIndex, msg.indexOf(",", dateIndex));
    state = msg.substring(stateIndex, msg.indexOf(",", stateIndex));
    
    // Extract latitude and longitude
    String location = msg.substring(locationIndex);
    int commaIndex = location.indexOf(",");
    latitude = location.substring(0, commaIndex).toFloat();
    longitude = location.substring(commaIndex + 1).toFloat();
    
    // Set the locked state flag
    isLocked = (state == "LOCKED");
}

void prepareTxFrame(uint8_t port) {
    // Prepare LoRaWAN frame here
    Serial.println("Preparing TX frame...");
}

void sendLocationUpdate() {
    Serial.println("Sending location update...");
    String jsonMessage = "{\"latitude\":" + String(latitude, 6) + ",\"longitude\":" + String(longitude, 6) + "}";
    
    // Copy the JSON message into the appData buffer for transmission
    strncpy((char*)appData, jsonMessage.c_str(), sizeof(appData));
    appDataSize = jsonMessage.length();
    deviceState = DEVICE_STATE_SEND;
}

void sendAlertMessage() {
    Serial.println("Sending alert message...");
    String alertMessage = "{\"alert\":\"Bike has been tampered with!\"}";

    // Copy the alert message into the appData buffer
    strncpy((char*)appData, alertMessage.c_str(), sizeof(appData));
    appDataSize = alertMessage.length();
    deviceState = DEVICE_STATE_SEND;
}