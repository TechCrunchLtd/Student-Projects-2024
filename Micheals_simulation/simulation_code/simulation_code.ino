
#include "project_header.h"

#define ON                          true
#define OFF                         false
#define PUMP                        13
#define DATA_SEND_INTERVAL_SECS     1
#define LOW_MOISTURE_THRESHOLD      40
#define HIGH_MOISTURE_THRESHOLD     90

DEVICE_PARAMS device_params;

unsigned long dataTime;

void sendData()
{
    String data = "{";
    data += "\"pumping\":" + String(device_params.pump_on) + ",";
    data += "\"humidity\":" + String(device_params.humidity) + ",";
    data += "\"moisture\":" + String(device_params.soil_moisture) + ",";
    data += "\"temperature\":" + String(device_params.temperature, 2) + "}";

    Serial.println(data);
}

void switchPump( bool new_state)
{
    if(new_state != digitalRead(PUMP))
    {
        digitalWrite(PUMP, new_state);
        device_params.pump_on = new_state;
    }
}

void setup()
{
    Serial.begin(9600);

    pinMode(PUMP, OUTPUT);
    switchPump(OFF);

    display_initialise();

    sensor_Init();
    
    dataTime = millis();
}

void loop()
{
    sensor_ReadValues();

    if(device_params.raining || (device_params.soil_moisture > HIGH_MOISTURE_THRESHOLD))
    {
        switchPump(OFF);
    }
    else if(!device_params.raining && (device_params.soil_moisture < LOW_MOISTURE_THRESHOLD))
    {
        switchPump(ON);
    }

    if((millis() - dataTime) > (DATA_SEND_INTERVAL_SECS * 1000))
    {
        sendData();
        dataTime = millis();
    }

    display_update(OFF);
}