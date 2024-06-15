
#include "Auto_Ambubag.h"

#define POTENTIOMETER   A0
#define POT_SAMPLES     10

void (*inputProcessor)(void) = NULL;

uint8_t readPotentiometer()
{
    int sum = 0;
    for(int i=0; i<POT_SAMPLES; i++)
    {
        sum += analogRead(POTENTIOMETER);
    }

    float average = (sum * 1.0) /POT_SAMPLES;

    return map((int)average, 50, 1023, 0, 10);
}

void buttonCallback()
{
    delay(100);
    if(digitalRead(BUTTON) == true)
    {
        if(!device_params.device_sleeping)
        {
            device_params.motor_running = !device_params.motor_running;

            inputProcessor();
        }
    }
}

bool input_setup(void (*inputCallback)(void))
{
    #ifdef DEVICE_DEBUG
        Serial.println("input setup");
    #endif

    inputProcessor = inputCallback;

    attachInterrupt(digitalPinToInterrupt(BUTTON), buttonCallback, RISING);

    return true;
}

void input_update()
{
    uint8_t pot_rate = readPotentiometer();
    device_params.motor_rate = pot_rate;
}

/* ----------------------------------- EOF ---------------------------------- */