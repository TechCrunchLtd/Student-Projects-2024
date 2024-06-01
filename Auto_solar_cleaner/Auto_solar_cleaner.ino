
#include "Auto_solar_cleaner.h"

#define RED_LED         10
#define GREEN_LED       9
#define BLUE_LED        8

typedef enum LED_COLOR
{
    NONE,
    RED,
    GREEN,
    BLUE
}
LED_COLOR;


typedef enum SYSTEM_CLEANING_STATE
{
    STATE_NO_CLEANING,
    STATE_START_CLEANING,
    STATE_FORWARD_CLEANING,
    STATE_BACKWARD_CLEANING,
    STATE_STOP_CLEANING
}
SYSTEM_CLEANING_STATE;

SYSTEM_CLEANING_STATE system_cleaning_state;

SOLAR_PARAMS solar_params;

unsigned long prevMillis;


void switchLed(LED_COLOR new_color)
{
    static LED_COLOR current_color = NONE;
    if(current_color != new_color)
    {
        digitalWrite(RED_LED, LOW);
        digitalWrite(BLUE_LED, LOW);
        digitalWrite(GREEN_LED, LOW);

        switch(new_color)
        {
            case RED:
                digitalWrite(RED_LED, HIGH);
            break;

            case BLUE:
                digitalWrite(BLUE_LED, HIGH);
            break;

            case GREEN:
                digitalWrite(GREEN_LED, HIGH);
            break;
        }

        current_color = new_color;
    }
}


void setup()
{
    bool init_error = false;

    Serial.begin(115200);

    if(driver_initialise() == false)
    {
        Serial.println("driver init failed");
        init_error = true;
    }

    if(cleaner_initialise() == false)
    {
        Serial.println("cleaner init failed");
        init_error = true;
    }

    if(monitor_initialise() == false)
    {
        Serial.println("monitor init failed");
        init_error = true;
    }

    while(init_error)
    {
        Serial.print(".");
        delay(3000);
    }

    system_cleaning_state = STATE_NO_CLEANING;

    Serial.println("System started");
}


void loop()
{
    switch(system_cleaning_state)
    {
        case STATE_NO_CLEANING:
            switchLed(GREEN);
            if(monitor_panelClean() == false)
            {
                system_cleaning_state = STATE_FORWARD_CLEANING;
                // system_cleaning_state = STATE_START_CLEANING;
                Serial.println("Panel needs cleaning");
            }
        break;

        case STATE_START_CLEANING:
            switchLed(RED);
            //move to start point
            if(driver_moveBackward() != false)
            {
                system_cleaning_state = STATE_FORWARD_CLEANING;
                Serial.println("Ready for forward cleaning");
            }
        break;

        case STATE_FORWARD_CLEANING:
            switchLed(RED);
            //cleaning begins
            if(cleaner_startCleaning() == false)
            {
                //stop movement if cleaning fails
                driver_moveStop();
                system_cleaning_state = STATE_STOP_CLEANING;
                Serial.println("Cleaning halted");
            }

            //move system forward
            if(driver_moveForward() != false)
            {
                system_cleaning_state = STATE_BACKWARD_CLEANING;
                Serial.println("Forward cleaning complete");
            }
        break;

        case STATE_BACKWARD_CLEANING:
            switchLed(BLUE);
            //cleaning continues
            if(cleaner_startCleaning() == false)
            {
                //stop movement if cleaning fails
                driver_moveStop();
                system_cleaning_state = STATE_STOP_CLEANING;
                Serial.println("Cleaning halted");
            }

            //move system backward
            if(driver_moveBackward() != false)
            {
                system_cleaning_state = STATE_STOP_CLEANING;
                Serial.println("Backward cleaning complete");
            }
        break;

        case STATE_STOP_CLEANING:
            switchLed(BLUE);
            //cleaning stops
            if(cleaner_stopCleaning() != false)
            {
                Serial.println("System failure");
            }
            //move back to start position
            // if(driver_moveBackward() != false)
            // {
            //     Serial.println("Cleaning completed");
            // }
                system_cleaning_state = STATE_NO_CLEANING;
        break;
    }

    if(millis() - prevMillis > 1000)
    {
        Serial.print("volt: "); Serial.println(solar_params.voltage);
        Serial.print("curr: "); Serial.println(solar_params.current);
        Serial.print("lght: "); Serial.println(solar_params.light);
        Serial.print("clin: "); Serial.println(solar_params.panel_is_clean);
        Serial.print("stat: "); Serial.println(system_cleaning_state);

        prevMillis = millis();
    }

}