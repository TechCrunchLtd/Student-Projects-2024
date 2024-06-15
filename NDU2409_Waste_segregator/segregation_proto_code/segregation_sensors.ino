
#include "segregation_proto.h"
#define INFRARED_SENSOR_PIN 13
#define METALLIC_SENSOR_PIN 9
#define PLASTIC_SENSOR_PIN 12


DEVICE_RESULT sensor_initialise(void)
{
    pinMode(PLASTIC_SENSOR_PIN, INPUT);
    pinMode(METALLIC_SENSOR_PIN, INPUT);
    pinMode(INFRARED_SENSOR_PIN, INPUT);
    return DEVICE_TRUE;
}

DEVICE_RESULT sensor_readInfrared(void)
{
    if(digitalRead(INFRARED_SENSOR_PIN) == LOW)
        return DEVICE_TRUE;
    return DEVICE_FALSE;
}

DEVICE_RESULT sensor_checkIfPaper(void)
{
    return DEVICE_FALSE;
}

DEVICE_RESULT sensor_checkIfMetallic(void)
{
    display_log("checking metal");
    delay(2000);
    if(digitalRead(PLASTIC_SENSOR_PIN) == LOW)
        return DEVICE_TRUE;
    return DEVICE_FALSE;
}

DEVICE_RESULT sensor_checkIfPlastic(void)
{
    display_log("checking plastic");
    delay(2000);
    if(digitalRead(METALLIC_SENSOR_PIN) == LOW)
        return DEVICE_TRUE;
    return DEVICE_FALSE;
}