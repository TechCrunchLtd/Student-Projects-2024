/**
 * @brief This file hamdles sensor that detect if a particular parking slot is occupied or not.
*/

#include "parking_monitor.h"
#define LED1        8
#define LED2        9
#define SENSOR1     10
#define SENSOR2     11

const int sensorLEDs[TOTAL_PARKING_SLOTS] = {LED1, LED2};
const int sensorPins[TOTAL_PARKING_SLOTS] = {SENSOR1, SENSOR2};

PARKING_STATE readSensorStatus(uint8_t sensor)
{
    if(digitalRead(sensorPins[sensor]) == false)
    {
        digitalWrite(sensorLEDs[sensor], HIGH);
        return SLOT_OCCUPIED;
    }

    digitalWrite(sensorLEDs[sensor], LOW);
    return SLOT_FREE;
}


void slot_sensorInit()
{
    for(int sensor=0; sensor<TOTAL_PARKING_SLOTS; sensor++)
    {
        pinMode(sensorPins[sensor], INPUT);
        pinMode(sensorLEDs[sensor], OUTPUT);
    }
}


void slot_sensorRead()
{
    for(int slot=0; slot<TOTAL_PARKING_SLOTS; slot++)
    {
        parking_params.parking_slots[slot] = readSensorStatus(slot);
    }
}

/* ----------------------------------- EOF ---------------------------------- */