
#include "segregator_config.h"

#define IR_SENSOR           2
#define ORGANIC_SENSOR      13
#define INDUCTIVE_SENSOR    3
#ifdef SIMULATION
#define CAPACITIVE_SENSOR   4
#else
#define CAPACITIVE_SENSOR   A0
#endif
#define ROBOT_ARM_1         5
#define ROBOT_ARM_2         6
#define ROBOT_MOVE_MILS     100

unsigned long robot_millis;

void moveObject()
{
    #ifdef DEVICE_DEBUG
        Serial.println("Moving Arm");
    #endif
    if(digitalRead(ROBOT_ARM_1) == false)
    {
        digitalWrite(ROBOT_ARM_1, HIGH);
    }
}

void stopObject()
{
    if(digitalRead(ROBOT_ARM_1) == true)
    {
        digitalWrite(ROBOT_ARM_1, LOW);
    }
}

bool metalDetected()
{
    return digitalRead(INDUCTIVE_SENSOR);
}

bool plasticDetected()
{
    #ifdef SIMULATION
        return digitalRead(CAPACITIVE_SENSOR);
    #else
        //in prototype read capacitive level
        int level = analogRead(CAPACITIVE_SENSOR);
        if(level > 500) return true;
    #endif

    return false;
}

bool organicDetected()
{
    return digitalRead(ORGANIC_SENSOR);
}

bool waste_detectionSetup()
{
    pinMode(IR_SENSOR, INPUT);
    pinMode(INDUCTIVE_SENSOR, INPUT);
    pinMode(CAPACITIVE_SENSOR, INPUT);
    pinMode(ORGANIC_SENSOR, INPUT);
    pinMode(ROBOT_ARM_1, OUTPUT);
    pinMode(ROBOT_ARM_2, OUTPUT);

    digitalWrite(ROBOT_ARM_1, LOW);
    digitalWrite(ROBOT_ARM_2, LOW);

    return true;
}

bool waste_checkPresence()
{
    static bool time_set = false;

    if(digitalRead(IR_SENSOR) == true)
    {
        if(!time_set)
        {
            robot_millis = millis();
            time_set = true;
            #ifdef DEVICE_DEBUG
                Serial.println("Time reset!");
            #endif
        }

        return true;
    }
    else
    {
        stopObject();
        time_set = false;
        return false;
    }

}

WASTE_TYPE waste_detectType()
{
    if(millis() - robot_millis < ROBOT_MOVE_MILS)
    {
        moveObject();
    }
    else
    {
        stopObject();

        if(metalDetected() == true)
        {
            #ifdef DEVICE_DEBUG
                Serial.println("Metal detected");
            #endif

            return WASTE_METALLIC;
        }

        if(plasticDetected() == true)
        {
            #ifdef DEVICE_DEBUG
                Serial.println("Plastic detected");
            #endif

            return WASTE_PLASTIC;
        }

        if(organicDetected() == true)
        {
            #ifdef DEVICE_DEBUG
                Serial.println("Organic detected");
            #endif

            return WASTE_ORGANIC;
        }
    }

    return WASTE_UNDEFINED;
}

