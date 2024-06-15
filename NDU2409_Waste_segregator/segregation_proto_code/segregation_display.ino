
#include "segregation_proto.h"
#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>

#define DISPLAY_INTERVAL    500
#define DISPLAY_COLS        16
#define DISPLAY_ROWS        2

LiquidCrystal_PCF8574 lcd(0x27);

unsigned long display_time;

extern ROBOT_LOCATION currentLocation;

DEVICE_RESULT display_initialise(void)
{
    Wire.begin();
    Wire.beginTransmission(0x27);
    int error = Wire.endTransmission();
    #ifdef DEBUG_DEVICE
    Serial.println("Di");
    #endif

    if(error != 0)
    {
        return DEVICE_ERROR;
    }

    lcd.begin(DISPLAY_COLS, DISPLAY_ROWS);
    lcd.setBacklight(255);

    return DEVICE_TRUE;
}

DEVICE_RESULT display_update(void)
{
    if(millis() - display_time < DISPLAY_INTERVAL)
        return DEVICE_FALSE;
    
    #ifdef DEBUG_DEVICE
    Serial.print("S: ");
    Serial.print(device_params.device_state);
    Serial.print(" P: ");
    Serial.println(device_params.current_position);
    Serial.print("W: ");
    Serial.println(device_params.current_waste);
    Serial.print(currentLocation.horizontal);Serial.print(", ");
    Serial.print(currentLocation.vertical); Serial.print(", ");
    Serial.println(currentLocation.rotational);
    #endif

    lcd.clear();
    lcd.print(millis()/1000);
    lcd.print(" ST: ");
    lcd.print(device_params.device_state);
    lcd.print(" <=> POS: ");
    lcd.print(device_params.current_position);
    lcd.setCursor(0,1);
    lcd.print("WASTE: ");
    switch(device_params.current_waste)
    {
        case NO_WASTE:
            lcd.print("NONE");
        break;
        
        case PAPER_WASTE:
            lcd.print("PAPER");
        break;
        case METALLIC_WASTE:
            lcd.print("METAL");
        break;
        case PLASTIC_WASTE:
            lcd.print("PLASTIC");
        break;
        case OTHER_WASTE:
            lcd.print("OTHER");
        break;
    }

    display_time = millis();

    return DEVICE_TRUE;
}

void display_log(char *msg)
{
    lcd.clear();
    // if(strlen(msg) > DISPLAY_COLS)
    // {
        // char temp[DISPLAY_COLS] = {0};
        // memcpy(temp, msg, DISPLAY_COLS);
        lcd.print(msg);
        lcd.setCursor(0,1);
        // lcd.print(msg+16);
        lcd.print(currentLocation.horizontal);lcd.print(", ");
        lcd.print(currentLocation.vertical); lcd.print(", ");
        lcd.print(currentLocation.rotational);
        

        return;
    // }

    // lcd.print(msg);
}

    /* ----------------------------------- EOF ---------------------------------- */