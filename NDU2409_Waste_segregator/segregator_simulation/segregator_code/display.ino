
#include "segregator_config.h"
#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>

#ifdef SIMULATION
#define DISPLAY_INTERVAL    3
#define DISPLAY_ADDRESS     0X38
#else
#define DISPLAY_INTERVAL    500
#define DISPLAY_ADDRESS     0X27
#endif

LiquidCrystal_PCF8574 lcd(DISPLAY_ADDRESS);

DEVICE_PARAMS display_params;


bool display_init()
{
    Wire.begin();
    Wire.beginTransmission(DISPLAY_ADDRESS);
    int error = Wire.endTransmission();

    if(error != 0)
    {
        return false;
    }

    lcd.begin(16, 2);
    lcd.setBacklight(255);

    lcd.clear();
    lcd.print("WASTE DETECTED>>");
    lcd.setCursor(0,1);
    lcd.print("  NO WASTE YET  ");

    return true;
}

void display_update()
{
    static unsigned long display_millis = 0;
    if(millis() - display_millis < DISPLAY_INTERVAL) return;
    display_millis = millis();

    if(display_params.waste_detected != device_params.waste_detected)
    {
        display_params.waste_detected = device_params.waste_detected;
        lcd.setCursor(0,1);
        lcd.print("                ");
        lcd.setCursor(0,1);

        switch(display_params.waste_detected)
        {
            case WASTE_NONE:
                lcd.print("  NO WASTE YET  ");
            break;

            case WASTE_UNDEFINED:
                lcd.print("Please wait...");
            break;

            case WASTE_ORGANIC:
                lcd.print("ORGANIC");
            break;

            case WASTE_METALLIC:
                lcd.print("METALLIC");
            break;

            case WASTE_PLASTIC:
                lcd.print("PLASTIC");
            break;
        }
    }
}
