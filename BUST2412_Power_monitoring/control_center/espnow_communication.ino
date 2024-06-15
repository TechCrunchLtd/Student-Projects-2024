
#include "device_config.h"

#include <ESP8266WiFi.h>
#include <espnow.h>

#define KEEP_ALIVE_SECS     2

/// @brief MAC address of the receiver device --> 5C:CF:7F:D9:86:30
uint8_t recieverAddress[] = {0x5C, 0xCF, 0x7F, 0xD9, 0x86, 0x30};


typedef struct LINE_PARAMS
{
    uint16_t voltage;
    uint16_t current;
    uint8_t fault;
    uint8_t line_status;
} LINE_PARAMS;

const char keepAliveMsg[] = "OK";
unsigned long dataSentMillis;
unsigned long dataRecvMillis;

uint8_t blinkRate = 0;

void getMACAddress()
{
    WiFi.mode(WIFI_STA);

    while(1)
    {
        Serial.println(WiFi.macAddress());
        delay(2000);
    }

}


void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus)
{
      
    if(sendStatus == 0)
    {
#ifdef DEVICE_DEBUG
    Serial.println("Sent keepAlive");
#endif
        dataSentMillis = millis();
    }
}

/// @brief Callme back for incoming data
/// @param mac address of the sender
/// @param incomingData pointer for received data
/// @param len total bytes of data received
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len)
{
    LINE_PARAMS line_params;
    // char line_data[20] = {0};
    // memset(line_data, '\0', 20);
    // memcpy(line_data, incomingData, len);
    memcpy(&line_params, incomingData, sizeof(line_params));
    // memcpy(&line_params, line_data, sizeof(line_params));

    #ifdef DEVICE_DEBUG
        Serial.println("---------------RECIEVED-------------");
        // Serial.println(line_data);
        Serial.println(line_params.voltage);
        Serial.println(line_params.current);
        Serial.println(line_params.fault);
        Serial.println(line_params.line_status);
        Serial.println("------------------------------------");
    #endif

    if(line_params.voltage >= 0 && line_params.voltage < 40000)
        device_params.line_voltage = (float)line_params.voltage / 100.0;
    if(line_params.current >= 0 && line_params.current < 30000)
        device_params.line_current = (float)line_params.current / 1000.0;
    if(line_params.fault >= 0 && line_params.fault < 6)
        device_params.line_fault = line_params.fault;
    if(line_params.line_status == 0 || line_params.line_status == 1)
        device_params.line_status = line_params.line_status;

    if(device_params.line_fault == 0)
    {
        //Clear line error
        if(device_params.device_error == LINE_ERROR)
            device_setError(NO_ERROR);
    }
    else
    {
        device_setError(LINE_ERROR);
    }

    dataRecvMillis = millis();
}

bool device_espnowStart( void )
{
    WiFi.mode(WIFI_STA); // Set device as a Wi-Fi Station
    WiFi.disconnect();

    // Init ESP-NOW
    if (esp_now_init() != 0) 
    {
        return false;
    }

    esp_now_set_self_role(ESP_NOW_ROLE_COMBO); // Set ESP-NOW Role

    // Once ESPNow is successfully Init, we will register for Send CB to
    // get the status of Trasnmitted packet
    esp_now_register_send_cb(OnDataSent);

    // Register peer
    esp_now_add_peer(recieverAddress, ESP_NOW_ROLE_COMBO, 1, NULL, 0);

    // Register for a callback function that will be called when data is received
    esp_now_register_recv_cb(OnDataRecv);

    dataSentMillis = millis();
    dataRecvMillis = millis();
    return true;
}

bool device_espnowStop( void )
{
    esp_now_deinit();
    return false;
}

bool device_espnowInit( void )
{
    // getMACAddress(); //run on first time to get device's address
    return device_espnowStart();
}

bool device_espnowUpdate( void )
{
    if(millis() - dataSentMillis > (KEEP_ALIVE_SECS*1000))
    {
#ifdef DEVICE_DEBUG
    Serial.println("Sending keepAlive");
#endif
        esp_now_send(recieverAddress, (uint8_t *) keepAliveMsg, strlen(keepAliveMsg));
        dataSentMillis = millis();
    }

    if(millis() - dataRecvMillis > (KEEP_ALIVE_SECS*3*1000))
    {
        return false;
    }
    
    //clear error due to esp communication failure
    if(device_params.device_error == COMM_ERROR)
        device_setError(NO_ERROR);

    return true;
}

/* ----------------------------------- EOF ---------------------------------- */