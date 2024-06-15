
#include "alchol_code.h"
#include <LiquidCrystal.h>
#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>
#include <ClickButton.h>

#define LCD_RS             12
#define LCD_EN             11
#define LCD_D4             10
#define LCD_D5              9
#define LCD_D6              8
#define LCD_D7              7
#ifdef SIMULATION_CODE
#define DISPLAY_INTERVAL  10
#else
#define DISPLAY_INTERVAL  100
#endif

LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

DEVICE_PARAMS display_params;
unsigned long display_time;
bool initial_display = false;

bool DISP_initialise()
{
    lcd.begin(16, 2);
    lcd.print("LCD READY");
    return true;
}

void DISP_update( void)
{   
    if(millis() - display_time < DISPLAY_INTERVAL) return;
    display_time = millis();

    if(strlen(device_params.log_msg) > 3)
    {
        lcd.clear();
        char temp[16] = {0};
        memcpy(temp, device_params.log_msg, 16);
        lcd.print(temp);
        if(strlen(device_params.log_msg) > 16)
        {
            lcd.setCursor(0,1);
            lcd.print(device_params.log_msg+16);
            display_time = millis();
        }
        return;
    }
    
    if(device_params.device_state == STATE_NORMAL)
    {
        lcd.clear();
        if(device_params.machine_running)
        {
            lcd.print("MACHINE RUNNING");
            lcd.setCursor(0,1);
            if(device_params.machine_direction)
            {
                lcd.print("REVERSE DIRECT..");
            }
            else
            {
                lcd.print("FORWARD DIRECT..");
            }
        }
        else
        {
            lcd.print("NO ALCOHOL");
            lcd.setCursor(0,1);
            lcd.print("OPERATOR ALLOWED");
        }
    }

}


/* ----------------------------------- EOF ---------------------------------- */