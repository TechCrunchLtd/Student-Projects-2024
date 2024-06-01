
#include "AWS_config.h"

/* ---------------------------- humidity library ---------------------------- */
// #include <Adafruit_Sensor.h>
#include <DHT.h>
// #include <DHT_U.h>

/* ------------------------------ temp library ------------------------------ */
#include <OneWire.h>
#include <DallasTemperature.h>

#define WATER_LEVEL_POWER       7
#define WATER_LEVEL_SENSOR      A0
#define HUMIDITY_SENSOR         2
#define TEMPERATURE_SENSOR      3
#define ANALOG_SAMPLES          10


#define DHTTYPE    DHT11
DHT dht(HUMIDITY_SENSOR, DHTTYPE);

// Setup a oneWire instance to communicate with any OneWire device
OneWire oneWire(TEMPERATURE_SENSOR);
DallasTemperature tempSensor(&oneWire);

bool sensors_setup()
{
    pinMode(WATER_LEVEL_POWER, OUTPUT);
    digitalWrite(WATER_LEVEL_POWER, LOW);
    dht.begin();
    tempSensor.begin();
    return true;
}

int getWaterLevel()
{
    digitalWrite(WATER_LEVEL_POWER, HIGH);
    delay(10);

    // int sum = 0;
    // for(int i=0; i<ANALOG_SAMPLES; i++)
    // {
    //     sum += analogRead(WATER_LEVEL_SENSOR);
    // }

    // int ana_val = sum/ANALOG_SAMPLES;
    int ana_val = analogRead(WATER_LEVEL_SENSOR);
    digitalWrite(WATER_LEVEL_POWER, LOW);

    return map(ana_val, 0, 1023, 0, 200);
}

bool sensors_read()
{
    tempSensor.requestTemperatures();
    int current_temperature = tempSensor.getTempCByIndex(0);
    #ifdef DEBUG_AWS_DEVICE
        Serial.print("Temp1:");
        Serial.println(current_temperature);
    #endif

    if(current_temperature > 0 && current_temperature < 255)
    {
        AWS_params.temperature = current_temperature;
    }
    
    float current_temp = dht.readTemperature();
    float current_humidity = dht.readHumidity();
    #ifdef DEBUG_AWS_DEVICE
        Serial.print("Temp2:");
        Serial.println(current_temp);
        Serial.print("Humd:");
        Serial.println(current_humidity);
    #endif
    
    if(!isnan(current_humidity))
    {
        AWS_params.humuidity = (uint8_t)current_humidity;
    }


    int water_level = getWaterLevel();
    #ifdef DEBUG_AWS_DEVICE
        Serial.print("Level:");
        Serial.println(water_level);
    #endif

    if(water_level >= 0 && water_level < 201)
    {
        AWS_params.water_level = water_level;
    }
    
}


