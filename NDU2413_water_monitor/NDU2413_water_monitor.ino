
#include "water_monitor.h"

#define BUZZER 6
#define PH_LOWER_LEVEL      6.5
#define PH_UPPER_LEVEL      9.0
#define TEMP_LOWER_LEVEL    20
#define TEMP_UPPER_LEVEL    32
#define DEPTH_LOWER_LEVEL   5
#define DEPTH_UPPER_LEVEL   10

WATER_PARAMS water_params;

void soundBuzzer(void)
{
    static unsigned long buzzer_millis = 0;
    if(millis() - buzzer_millis > 1500)
    {
        // tone(BUZZER, 4000, 100);
        analogWrite(BUZZER, 180);
        delay(60);
        analogWrite(BUZZER, 0);
        buzzer_millis = millis();
    }
}

void setup()
{
    #ifdef DEBUG_WATER_MONITOR
        Serial.begin(9600);
    #endif

    pinMode(BUZZER, OUTPUT);

    display_init();
    sensors_init;

    #ifdef DEBUG_WATER_MONITOR
        Serial.println("Init complete");
    #endif
}


void loop()
{
    sensors_readValues();
    display_update();

    if(
        (water_params.ph_level    < PH_LOWER_LEVEL)   ||
        (water_params.ph_level    > PH_UPPER_LEVEL)   ||
        (water_params.temperature < TEMP_LOWER_LEVEL) ||
        (water_params.temperature > TEMP_UPPER_LEVEL) ||
        (water_params.depth       < DEPTH_LOWER_LEVEL)||
        (water_params.depth       > DEPTH_UPPER_LEVEL) 
    )
    {
        soundBuzzer();
    }
}