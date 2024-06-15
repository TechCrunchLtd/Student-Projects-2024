

#include "segregator_config.h"

DEVICE_PARAMS device_params;

void setup()
{
    #ifdef DEVICE_DEBUG
        Serial.begin(9600);
        Serial.println("System started!");
    #endif

    display_init();

}


void loop()
{
    if(waste_checkPresence() == true)
    {
        device_params.waste_detected = waste_detectType();
        #ifdef DEVICE_DEBUG
            Serial.println("Waste detected");
        #endif
    }
    else
    {
        device_params.waste_detected = WASTE_NONE;
    }

    display_update();
}