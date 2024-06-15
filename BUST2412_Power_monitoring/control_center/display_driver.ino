#include "device_config.h"
#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>

LiquidCrystal_PCF8574 lcd(0x27);

bool device_displayInit( void )
{
	Wire.begin();
    Wire.beginTransmission(0x27);
    int error = Wire.endTransmission();

    if (error == 0) {
        #ifdef DEVICE_DEBUG
            Serial.println("LCD found");
        #endif

        lcd.begin(20, 4);  // initialize the lcd
        lcd.setBacklight(255);

        return true;

    } else {
        #ifdef DEVICE_DEBUG
            Serial.println("LCD not found");
        #endif

        return false;
    }
}

void device_displayUpdate(bool config)
{
	static unsigned long disp_millis = 0;
	if(millis() - disp_millis < 800) return;

	lcd.clear();

    if(config)
    {
        lcd.print(device_params.server_IP);
        lcd.setCursor(0,1);
        lcd.print(device_params.server_name);
        lcd.setCursor(0,2);
        lcd.print(device_params.user_number);
        lcd.setCursor(0,3);
        lcd.print(device_params.error_message);
        delay(10);
        return;
    }

	lcd.print("Current: ");
	lcd.print(device_params.line_current,2);
	lcd.print("A");
	
	lcd.setCursor(0,1);
	lcd.print("Voltage: ");
	lcd.print(device_params.line_voltage,2);
	lcd.print("V");

	lcd.setCursor(0,2);
    lcd.print("line A01: ");
    lcd.print(device_params.line_status?"ON":"OFF");

	lcd.setCursor(0,3);

    if(device_params.line_fault == 0)
    {
        lcd.print("NO FAULT");
    }
    else
    {
	    lcd.print(device_params.error_message);
    }
}

void device_displayError( char *error )
{
	lcd.clear();
	lcd.print(error);
}

/* ----------------------------------- EOF ---------------------------------- */