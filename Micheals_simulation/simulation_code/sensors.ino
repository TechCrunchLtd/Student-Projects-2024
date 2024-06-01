

#include "project_header.h"
#include <DHT.h>

#define RAIN_SENSOR A0
#define MOSITURE_SENSOR A1
#define HUMIDITY_SENSOR 3
#define DHTTYPE    DHT11
DHT dht(HUMIDITY_SENSOR, DHTTYPE);

bool sensor_Init()
{
    dht.begin();

    return true;
}

void sensor_ReadValues()
{
    //read temp and humidity
    float current_temp = dht.readTemperature();
    float current_humidity = dht.readHumidity();
    if(!isnan(current_humidity))
    {
        device_params.humidity = (uint8_t)current_humidity;
    }
    if(!isnan(current_temp))
    {
        device_params.temperature = current_temp;
    }

    //read soil moiture
    int moisture = 0;
    for(int i=0; i<SENSOR_SAMPLES; i++)
    {
        int val = analogRead(MOSITURE_SENSOR);
        if(moisture < val)
            moisture = val;
    }
    device_params.soil_moisture = map(moisture, 0, 1023, 0, 100);

    //read rain
    device_params.raining = digitalRead(RAIN_SENSOR);

}