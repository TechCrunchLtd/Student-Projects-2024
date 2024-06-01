
#include "AWS_config.h"
#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>

#define DEGREE      1
#define DISPLAY_INTERVAL_SECS    3

LiquidCrystal_PCF8574 lcd(0x27);

byte degrees[] = { 0b11100, 0b10100, 0b11100, 0b00000,
                 0b00000, 0b00000, 0b00000, 0b00000 };


bool display_setup()
{
    Wire.begin();
    Wire.beginTransmission(0x27);
    int error = Wire.endTransmission();

    if (error == 0) {
        #ifdef DEBUG_AWS_DEVICE
            Serial.println("LCD found");
        #endif

        lcd.begin(16, 2);  // initialize the lcd

        lcd.createChar(DEGREE, degrees);
        

        lcd.setBacklight(255);

        return true;

    } else {
        #ifdef DEBUG_AWS_DEVICE
            Serial.println("LCD not found");
        #endif

        return false;
    } 
    
}

bool display_sensor_vallues;
unsigned long display_millis;
void display_update()
{
    if((millis() - display_millis) < (DISPLAY_INTERVAL_SECS*1000)) return;

    display_millis = millis();

    if(display_sensor_vallues == true)
    {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Temp: ");
        lcd.print(AWS_params.temperature);
        lcd.write(DEGREE);
        lcd.setCursor(0,1);
        lcd.print("Humidity: ");
        lcd.print(AWS_params.humuidity);
        lcd.print("%");

    }
    else
    {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Pump is ");
        lcd.print(AWS_params.pumpOn ? "ON" : "OFF");
        lcd.setCursor(0,1);
        lcd.print("Peltier is ");
        lcd.print(AWS_params.peltierOn ? "ON" : "OFF");
    }

    display_sensor_vallues = !display_sensor_vallues;
}


/* ----------------------------------- EIF ---------------------------------- */