
#include "Auto_solar_cleaner.h"

#define WATER_PUMP              13
#define STEPPER_DIR             12
#define STEPPER_STEP            11
#define USER_INPUT_POT          A1

void waterpump(bool new_state)
{
    static bool pump_state = false;
    if(new_state != pump_state)
    {
        pump_state = new_state;
        digitalWrite(WATER_PUMP, pump_state);
        Serial.println("pump toggled");
    }
}

void rotateBrush(void)
{
    static bool stepped = false;
    static unsigned long prev_time = 0;
    static unsigned long motor_rate = 0;

    if(micros() - prev_time > motor_rate)
    {
        digitalWrite(STEPPER_STEP, stepped);
        stepped = !stepped;
        prev_time = micros();
    }

    motor_rate = map(analogRead(A1), 400, 1023, 300, 2000);
}

bool cleaner_initialise(void)
{
    pinMode(WATER_PUMP, OUTPUT);
    pinMode(STEPPER_DIR, OUTPUT);
    pinMode(STEPPER_STEP, OUTPUT);
    
    digitalWrite(STEPPER_DIR, LOW);
    waterpump(false);
    return true;
}

bool cleaner_startCleaning(void)
{
    waterpump(true);
    rotateBrush();
    return true;
}

bool cleaner_stopCleaning(void)
{
    solar_params.panel_is_clean = true;
    
    waterpump(false);
    return false;
}

/* ----------------------------------- EOF ---------------------------------- */