
#include "AWS_config.h"

#define PUMP        8
#define PELTIER     9
#define HUMIDITY_THRESHOLD      50
#define WATER_LOW_THRESHOLD     40
#define WATER_HIGH_THRESHOLD    60
#define TEMP_LOW_THRESHOLD      0
#define TEMP_HIGH_THRESHOLD     10

AWS_PARAMS AWS_params;

void turnPump(bool state)
{
    if(AWS_params.pumpOn != state)
    {
        digitalWrite(PUMP, state);
        AWS_params.pumpOn = state;
    }
}

void turnPlettier(bool state)
{
    if(AWS_params.peltierOn != state)
    {
        digitalWrite(PELTIER, state);
        AWS_params.peltierOn = state;
    }
}

void initOutputs()
{
    pinMode(PUMP, OUTPUT);
    pinMode(PELTIER, OUTPUT);

    AWS_params.pumpOn = true;
    AWS_params.peltierOn = true;

    turnPump(LOW);
    turnPlettier(LOW);
}

void setup()
{
    #ifdef DEBUG_AWS_DEVICE
        Serial.begin(115200);
    #endif

    initOutputs();

    display_setup();
    sensors_setup();

    /*for(int i=0; i<10; i++)
    {
        turnPlettier(true);
        turnPump(false);
        delay(1000);
        turnPlettier(false);
        turnPump(true);
        delay(1000);
    }*/

}

void loop()
{
    sensors_read();

    if((AWS_params.water_level < WATER_LOW_THRESHOLD) && 
        (AWS_params.humuidity  > HUMIDITY_THRESHOLD))
    {
        turnPump(HIGH);
    }
    else if(AWS_params.water_level > WATER_HIGH_THRESHOLD)
    {
        turnPump(LOW);
    }

    if(AWS_params.temperature <= TEMP_LOW_THRESHOLD)
    {
        turnPlettier(LOW);
    }
    else if(AWS_params.temperature > TEMP_HIGH_THRESHOLD)
    {
        turnPlettier(HIGH);
    }

    display_update();
    delay(100);
}

/* ----------------------------------- EOF ---------------------------------- */
