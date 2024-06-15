
#include "water_monitor.h"
#include <NewPing.h>
#include <DallasTemperature.h>
#include <OneWire.h>

#define TEMP_SENSOR     7
#define PH_SENSOR       A1
#define TRIG_PIN        2
#define ECHO_PIN        3
#define MAX_DISTANCE    50
#define PH_FACTOR       28.94

OneWire oneWire(TEMP_SENSOR);
DallasTemperature tempSensor(&oneWire);
NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE);

uint8_t read_cycle = 0;

void readTemp(void)
{
    tempSensor.requestTemperatures();

    float temp = tempSensor.getTempCByIndex(0);
    #ifdef DEBUG_WATER_MONITOR
        Serial.print("temp: ");
        Serial.println(temp);
    #endif

    if(temp > 0.0 && temp < 180.0)
    {
        water_params.temperature = (uint8_t)temp;
    }
    else{
        water_params.temperature = 0;
    }
}

void readPH(void)
{
    int buffer_arr[10], tempval;
    for(int i=0; i<10; i++) 
    { 
        buffer_arr[i] = analogRead(PH_SENSOR);
        delay(10);
    }

    for(int i=0;i<9;i++)
    {
        for(int j=i+1;j<10;j++)
        {
            if(buffer_arr[i]>buffer_arr[j])
            {
                tempval = buffer_arr[i];
                buffer_arr[i] = buffer_arr[j];
                buffer_arr[j] = tempval;
            }
        }
    }

    unsigned long avgval = 0;
    for(int i=2; i<8; i++)
        avgval += buffer_arr[i];

    float volt = (float)avgval*5.0/1024/6;
    float ph_act = -5.70 * volt + PH_FACTOR;
    #ifdef DEBUG_WATER_MONITOR
        Serial.print("ph: ");
        Serial.println(ph_act);
    #endif
    if(ph_act > 0.0 && ph_act < 180.0)
    {
        water_params.ph_level = ph_act;
    }
    else
    {
        water_params.ph_level = 0.0;
    }
}

void readLevel(void)
{
    static unsigned long lastping = 0;
    while(millis() - lastping < 50){delay(10);} //wait for 50ms between pings
    lastping = millis();
    uint16_t distance = sonar.ping_cm();
    #ifdef DEBUG_WATER_MONITOR
        Serial.print("wl: ");
        Serial.println(distance);
    #endif

    if(distance > 0 && distance < 100)
    {
        water_params.depth = distance;
    }
    else
    {
        water_params.ph_level = 0;
    }
}

void sensors_init(void)
{
    tempSensor.begin();
}

void sensors_readValues(void)
{
    if(read_cycle == 0) readTemp();
    else if(read_cycle == 1) readPH();
    else if(read_cycle == 2) readLevel();

    read_cycle = ++read_cycle%3;
}