#include <NewPing.h>
#include "parking_monitor.h"

#define EXIT_TRIG           2
#define EXIT_ECHO           3
#define ENTRANCE_TRIG       5
#define ENTRANCE_ECHO       6
#define MAXIMUM_DISTANCE    50
#define ENTRANCE_DISTANCE   20
#define EXIT_DISTANCE   7

NewPing Entrance(ENTRANCE_TRIG, ENTRANCE_ECHO, MAXIMUM_DISTANCE);
NewPing Exit(EXIT_TRIG, EXIT_ECHO, MAXIMUM_DISTANCE);


bool gate_checkEntranceRequest()
{
    uint8_t distance = Entrance.ping_cm();
    #ifdef DEBUG_PARKING
        Serial.print("Entrance: ");
        Serial.println(distance);
    #endif

    if(distance < ENTRANCE_DISTANCE && distance > 0)
    {
        return true;
    }

    return false;
}

bool gate_checkExitRequest()
{
    uint8_t distance = Exit.ping_cm();
    #ifdef DEBUG_PARKING
        Serial.print("Exit: ");
        Serial.println(distance);
    #endif

    if(distance < EXIT_DISTANCE && distance > 0)
    {
        return true;
    }

    return false;
}

