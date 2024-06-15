
#include "Auto_Ambubag.h"
#include <Sleep_n0m1.h>

#define DEVICE_SLEEP_SECS 110

DEVICE_PARAMS device_params;

Sleep sleep;

void (*softReset)(void) = 0;
unsigned long sleep_wait = 0;

void resetSleepTime()
{
    sleep_wait = millis();
}

void handleError( DEVICE_ERROR error)
{
    #ifdef DEVICE_DEBUG
        Serial.println("Error occured!!");
    #endif

    if(error == DISPLAY_ERROR)
    {
        #ifdef DEVICE_DEBUG
            Serial.println("Dislpay failed!");
        #endif
        
        softReset();
    }

    device_params.device_error = error;
    display_update(true);
}

void processInput()
{
    #ifdef DEVICE_DEBUG
        Serial.println("input received");
    #endif
    //read pot when button pressed
    if(device_params.motor_running)
    {
        input_update();
    }

    resetSleepTime();
}

void justSleep()
{
    motor_stop();
    device_params.sleep_time = DEVICE_SLEEP_SECS - ((millis() - sleep_wait) / 1000);

    if(device_params.sleep_time <=0)
    {
        //enter sleep
        #ifdef DEVICE_DEBUG
            Serial.println("Entering sleep");
        #endif

        display_clear();

        device_params.device_sleeping = true;

        sleep.pwrDownMode(); //set sleep mode
        sleep.sleepPinInterrupt(BUTTON, HIGH);

        #ifdef DEVICE_DEBUG
            Serial.println("Woke from sleep");
        #endif

        setup();
        device_params.device_sleeping = false;

    }
    else
    {
        #ifdef DEVICE_DEBUG
            Serial.println("Waiting for sleep time");
        #endif
        // delay(1000);
    }
}

void resuscitate()
{
    motor_run();

    #ifdef DEVICE_DEBUG
        Serial.println("Resuscitating!");
    #endif
    input_update();
    
    resetSleepTime();
}

void setup()
{
    #ifdef DEVICE_DEBUG
        Serial.begin(115200);
        Serial.println("starting...");
    #endif

    if(!display_setup())
    {
        handleError(DISPLAY_ERROR);
    }
    
    if(!input_setup(&processInput))
    {
        handleError(INPUT_ERROR);
    }

    motor_setUp();

    sleep_wait = millis();
}

void loop()
{
    if(device_params.motor_running)
    {
        resuscitate();
    }
    else 
    {
        justSleep();
    }

    display_update(false);
}