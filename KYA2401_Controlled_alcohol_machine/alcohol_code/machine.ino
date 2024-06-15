
#include "alchol_code.h"

#define MACHINE_REVERSE     5
#define MACHINE_FORWARD     4
#define MACHINE_CONTROL     3
#define MACHINE_DIRECTION   A1


void machineStop(void)
{
    digitalWrite(MACHINE_CONTROL, LOW);
    digitalWrite(MACHINE_REVERSE, LOW);
    digitalWrite(MACHINE_FORWARD, LOW);

    device_params.machine_running = false;
    Serial.println("Stop");
}


void machineRun(void)
{
    static bool reverse_direction = false;
    if(digitalRead(MACHINE_DIRECTION) != reverse_direction)
    {
        reverse_direction = digitalRead(MACHINE_DIRECTION);
        device_params.machine_direction = reverse_direction;
        #ifdef DEBUG_DEVICE
        Serial.println(analogRead(MACHINE_DIRECTION));
        #endif
    }
    digitalWrite(MACHINE_CONTROL, HIGH);

    if(device_params.machine_direction == HIGH)
    {
        digitalWrite(MACHINE_REVERSE, HIGH);
        digitalWrite(MACHINE_FORWARD, LOW);
        #ifdef DEBUG_DEVICE
        Serial.print("Reverse ");
        #endif
    }
    else
    {
        digitalWrite(MACHINE_REVERSE, LOW);
        digitalWrite(MACHINE_FORWARD, HIGH);
        #ifdef DEBUG_DEVICE
        Serial.print("Forward ");
        #endif
    }

    // digitalWrite(MACHINE_REVERSE, HIGH);
    // digitalWrite(MACHINE_FORWARD, LOW);

    device_params.machine_running = true;
    #ifdef DEBUG_DEVICE
    Serial.print("Dir: "); Serial.println(device_params.machine_direction);
    #endif
}

void machineInit(void)
{
    pinMode(MACHINE_CONTROL, OUTPUT);
    pinMode(MACHINE_FORWARD, OUTPUT);
    pinMode(MACHINE_REVERSE, OUTPUT);
    pinMode(MACHINE_DIRECTION, INPUT);

    machineStop();
}

/* ----------------------------------- EOF ---------------------------------- */