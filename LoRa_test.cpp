
#include "LoRaWan_APP.h"
#include <Arduino.h>

// Device EUI, App EUI, and App Key for OTAA (Over-The-Air Activation)
uint8_t devEui[] = { 0x70, 0xB3, 0xD5, 0x7E, 0xD0, 0x06, 0xB7, 0xD3 };
uint8_t appEui[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04 };
uint8_t appKey[] = { 0xC0, 0x36, 0x17, 0x85, 0xD2, 0x1A, 0xE7, 0xC8, 0x3D, 0xFE, 0x1C, 0x27, 0xCA, 0x68, 0x34, 0x78 };

uint8_t nwkSKey[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t appSKey[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint32_t devAddr = (uint32_t)0x00000000;

bool overTheAirActivation = true;       
uint16_t userChannelsMask[6] = { 0xFF00, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 };
LoRaMacRegion_t loraWanRegion = ACTIVE_REGION;
DeviceClass_t loraWanClass = CLASS_A;
uint32_t appTxDutyCycle = 30 * 1000; // ***** 30 seconds for testing, actual model will have device sleep after every 15 minutes *****
bool loraWanAdr = true;                
bool isTxConfirmed = true;            
uint8_t appPort = 1;                  
uint8_t confirmedNbTrials = 8;           


RTC_DATA_ATTR bool firstrun = true;

/* Reads temperature from the Heltec board's sensor
   Replace this mock function with actual code to read from the sensor */
int readTemperature() {
    int temperature = 2555;  // Mock value for 25.55°C, adjust based on actual reading
    return temperature;
}

/* Prepares the payload of the frame */
static void prepareTxFrame(uint8_t port) {
    appDataSize = 10;  // 1 byte for state, 4 bytes for latitude, 4 bytes for longitude

    // State (1 byte: LOCKED = 1, UNLOCKED = 0)
    appData[0] = 1;  // 1 represents LOCKED

    // Location: Latitude and Longitude (encoded as 4-byte integers)
    int32_t latitude = (int32_t)(42.089654 * 1000000);   // Scale to 6 decimal places
    int32_t longitude = (int32_t)(-75.970346 * 1000000); // Scale to 6 decimal places

    // Encode latitude (MSB first)
    appData[1] = (latitude >> 24) & 0xFF;
    appData[2] = (latitude >> 16) & 0xFF;
    appData[3] = (latitude >> 8) & 0xFF;
    appData[4] = latitude & 0xFF;

    // Encode longitude (MSB first)
    appData[5] = (longitude >> 24) & 0xFF;
    appData[6] = (longitude >> 16) & 0xFF;
    appData[7] = (longitude >> 8) & 0xFF;
    appData[8] = longitude & 0xFF;

    // Optionally, add other fields as needed
}

void setup() {
    Serial.begin(115200);
    Mcu.begin();

    if (firstrun) {
        LoRaWAN.displayMcuInit();
        firstrun = false;
    }
    deviceState = DEVICE_STATE_INIT;
}

void loop() {
    switch (deviceState) {
        case DEVICE_STATE_INIT: {
#if (LORAWAN_DEVEUI_AUTO)
            LoRaWAN.generateDeveuiByChipID();
#endif
            LoRaWAN.init(loraWanClass, loraWanRegion);
            break;
        }
        case DEVICE_STATE_JOIN: {
            LoRaWAN.displayJoining();
            LoRaWAN.join();
            if (deviceState == DEVICE_STATE_SEND) {
                LoRaWAN.displayJoined();
            }
            break;
        }
        case DEVICE_STATE_SEND: {
            LoRaWAN.displaySending();
            prepareTxFrame(appPort);  // Prepare the payload with group and temperature data
            LoRaWAN.send();
            deviceState = DEVICE_STATE_CYCLE;
            break;
        }
        case DEVICE_STATE_CYCLE: {
            // Schedule the next packet transmission
            txDutyCycleTime = appTxDutyCycle + randr(0, APP_TX_DUTYCYCLE_RND);
            LoRaWAN.cycle(txDutyCycleTime);
            deviceState = DEVICE_STATE_SLEEP;
            break;
        }
        case DEVICE_STATE_SLEEP: {
            LoRaWAN.displayAck();
            LoRaWAN.sleep(loraWanClass);
            break;
        }
        default: {
            deviceState = DEVICE_STATE_INIT;
            break;
        }
    }
}
