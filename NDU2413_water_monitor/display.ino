
#include "water_monitor.h"
#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>

#define DISPLAY_INTERVAL    500

LiquidCrystal_PCF8574 lcd(0x27);
WATER_PARAMS display_params;
unsigned long display_millis;

void display_init(void)
{
    Wire.begin();
    Wire.beginTransmission(0x27);
    int error = Wire.endTransmission();

    lcd.begin(16, 2);
    lcd.setBacklight(255);

    lcd.clear();
    lcd.print("TEMP   PH  LEVEL");
    lcd.print("27C  14.14  50cm");
    lcd.setCursor(0,1);

    display_params.depth = 50;
    display_params.ph_level = 14.0;
    display_params.temperature = 100;
}


void display_update(void)
{
    if(millis() - display_millis < DISPLAY_INTERVAL) return;

    display_millis = millis();

    if(display_params.temperature != water_params.temperature)
    {
        display_params.temperature = water_params.temperature;
        lcd.setCursor(0,1);
        lcd.print("     ");
        lcd.setCursor(0,1);
        lcd.print(water_params.temperature);
        lcd.print("C");
        
    }

    if(display_params.ph_level != water_params.ph_level)
    {
        display_params.ph_level = water_params.ph_level;
        lcd.setCursor(5,1);
        lcd.print("      ");
        lcd.setCursor(5,1);
        lcd.print(water_params.ph_level);
    }

    if(display_params.depth != water_params.depth)
    {
        display_params.depth = water_params.depth;
        lcd.setCursor(11,1);
        lcd.print("     ");
        lcd.setCursor(11,1);
        lcd.print(water_params.depth);
    }


}
