
#include "Auto_Ambubag.h"
#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>

#define DISPLAY_INTERVAL    500

LiquidCrystal_PCF8574 lcd(0x27);

DEVICE_PARAMS display_params;

unsigned long display_time;

bool display_setup()
{
    #ifdef DEVICE_DEBUG
        Serial.println("display setup");
    #endif

    Wire.begin();
    Wire.beginTransmission(0x27);
    int error = Wire.endTransmission();

    if(error != 0)
    {
        return false;
    }

    lcd.begin(16, 2);
    lcd.setBacklight(255);
    lcd.display();

    display_params.motor_rate = 10;
    display_params.device_sleeping = false;
    return true;
}

void display_clear()
{
    lcd.clear();
    lcd.setBacklight(0);
    lcd.noDisplay();
}

void display_update(bool on_error)
{
    if(millis() - display_time < DISPLAY_INTERVAL) return;

    display_time = millis();
    
    if(on_error)
    {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("ERROR OCCURED!!!");
        lcd.setCursor(0,1);
        lcd.print(device_params.error_message);
    }
    else
    {
        if(device_params.motor_running)
        {
            if((display_params.motor_rate != 10) && (device_params.motor_rate == display_params.motor_rate))
            {
                return;
            }

            display_params.motor_rate = device_params.motor_rate;
            display_params.device_sleeping = false;
            
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.println(" RESUSCINATING  ");
            lcd.setCursor(0,1);
            lcd.print("SET TO: ");
            int tidal_volume = map(display_params.motor_rate,0,10,1000,0);
            lcd.print(tidal_volume);
            lcd.print("mL");
            
        }
        else
        {
            if(display_params.device_sleeping)
            {
                if(display_params.sleep_time != device_params.sleep_time)
                {
                    lcd.setCursor(12,1);
                    lcd.print("    ");
                    lcd.setCursor(12,1);
                    lcd.print(device_params.sleep_time);
                    lcd.print("s");
                    
                    display_params.sleep_time = device_params.sleep_time;
                }

                return;
            }
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("DEVICE ON PAUSE");
            lcd.setCursor(0,1);
            lcd.print("SLEEP TIME: Secs");
            lcd.print(device_params.sleep_time);
            lcd.print("s");
            display_params.motor_rate = 10;
            display_params.device_sleeping = true;
        }
        
    }
}


/* ----------------------------------- EOF ---------------------------------- */