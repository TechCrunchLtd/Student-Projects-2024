
#include "project_header.h"
#include <LiquidCrystal.h>

#define LCD_RS             12
#define LCD_EN             11
#define LCD_D4              7
#define LCD_D5              6
#define LCD_D6              5
#define LCD_D7              4
#ifdef SIMULATION_CODE
#define DISPLAY_INTERVAL  10
#else
#define DISPLAY_INTERVAL  500
#endif

LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

DEVICE_PARAMS display_params;
unsigned long display_time;
bool initial_display = false;

bool display_initialise()
{
    lcd.begin(16, 2);
    return true;
}

void display_update( bool log)
{   
    // lcd.clear();
    // lcd.print("->");lcd.print(analogRead(A0));
    // lcd.print("->");lcd.print(analogRead(A1));
    // lcd.print("->");lcd.print(analogRead(A5));
    // return;

    if(log)
    {
        lcd.clear();
        lcd.print(device_params.log_msg);
        display_time = millis();
        return;
    }

    if(millis() - display_time < DISPLAY_INTERVAL) return;
    display_time = millis();

    if(initial_display == false)
    {
        display_params.pump_on = false;
        display_params.raining = false;
        display_params.temperature = 0.0;
        display_params.humidity = 0;
        display_params.soil_moisture = 0;
        
        lcd.clear();
        lcd.print("TP: 0.0 ");
        lcd.print("  HD: 0");
        lcd.setCursor(0,1);
        lcd.print("SM: 0   ");
        lcd.print("PUMP OFF");
        initial_display = true;
        return;
    }


    if(device_params.temperature != display_params.temperature)
    {
        display_params.temperature = device_params.temperature;
        lcd.setCursor(3,0);
        lcd.print("     ");
        lcd.setCursor(3,0);
        lcd.print(display_params.temperature,2);
        lcd.print("C");
    }

    if(device_params.humidity != display_params.humidity)
    {
        display_params.humidity = device_params.humidity;
        lcd.setCursor(13,0);
        lcd.print("     ");
        lcd.setCursor(13,0);
        lcd.print(display_params.humidity);
        lcd.print("%");
    }

    if(device_params.soil_moisture != display_params.soil_moisture)
    {
        display_params.soil_moisture = device_params.soil_moisture;
        lcd.setCursor(3,1);
        lcd.print("     ");
        lcd.setCursor(3,1);
        lcd.print(display_params.soil_moisture);
        lcd.print("%");
    }

    if(device_params.pump_on != display_params.pump_on)
    {
        display_params.pump_on = device_params.pump_on;
        lcd.setCursor(8,1);
        lcd.print("         ");
        lcd.setCursor(8,1);
        lcd.print("PUMP ");
        lcd.print(display_params.pump_on?"ON":"OFF");
    }

}


/* ----------------------------------- EOF ---------------------------------- */