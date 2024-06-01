
#include "gsm_home.h"

#define LOAD 13
#define BUTTON 7

HOME_PARAMS home_params;

void switchLoad(HOME_CMD new_cmd)
{
    switch(new_cmd)
    {
        case NO_CMD:
        case LOAD_OFF:
            digitalWrite(LOAD, LOW);
            home_params.loadon = LOAD_OFF;
        break;

        case LOAD_ON:
            digitalWrite(LOAD, HIGH);
            home_params.loadon = LOAD_ON;
        break;

        case LOAD_TOGGLE:
            if(home_params.loadon == LOAD_ON)
            {
                digitalWrite(LOAD, LOW);
                home_params.loadon = LOAD_OFF;
            }
            else
            {
                digitalWrite(LOAD, HIGH);
                home_params.loadon = LOAD_ON;
            }
        break;
    }
}

void setup()
{
    pinMode(LOAD, OUTPUT);

    display_init();
    display_log("Waiting on GSM");
    gsm_init();
    display_log("System Ready");

    switchLoad(LOAD_OFF);
}

void loop()
{
    HOME_CMD gsm_cmd = NO_CMD;
    //read button
    if(digitalRead(BUTTON) != false)
    {
        switchLoad(LOAD_TOGGLE);
        delay(1500);
    }

    //read gsm
    gsm_cmd = gsm_run();
    if(gsm_cmd != NO_CMD)
    {
        switchLoad(gsm_cmd);
    }

    display_update();
}