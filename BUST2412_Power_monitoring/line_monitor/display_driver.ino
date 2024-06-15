#include "line_monitor.h"
#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>

LiquidCrystal_PCF8574 lcd(0x27);

bool device_displayInit( void )
{
	Wire.begin();
    Wire.beginTransmission(0x27);
    int error = Wire.endTransmission();

    if (error == 0) 
    {
        lcd.begin(20, 4);  // initialize the lcd
        lcd.setBacklight(255);
        lcd.print("Ready...");

        return true;

    } else 
    {
        return false;
    }
}

void device_displayUpdate( void )
{
	static unsigned long disp_millis = 0;
	if(millis() - disp_millis < 800) return;

	lcd.clear();
	lcd.print("Current: ");
	lcd.print(line_params.current);
	lcd.print("A");
	
	lcd.setCursor(0,1);
	lcd.print("Voltage: ");
	lcd.print(line_params.voltage);
	lcd.print("V");

	lcd.setCursor(0,2);
    lcd.print("line A01: ");
    lcd.print((line_params.line_status==1)?"ON":"OFF");

	lcd.setCursor(0,3);
    switch(line_params.fault)
    {
        case NO_FAULT:
	        lcd.print("No Fault");
        break;

        case SHORT_CIRCUIT:
	        lcd.print("Short Circuit");
        break;
        case OVER_LOAD:

	        lcd.print("Over load");
        break;

        case UNDER_VOLTAGE:
	        lcd.print("Under Voltage");
        break;

        case OVER_VOLTAGE:
	        lcd.print("Over Voltage");
        break;

        case OPEN_CIRCUIT:
	        lcd.print("Open Circuit");
        break;
    }
}

void device_displayNotify(char *msg)
{
    LINE_PARAMS sent_params;
    memcpy(&sent_params, msg, sizeof(sent_params));
    lcd.clear();
    lcd.print("sent: ");
    lcd.print(sent_params.voltage);
    lcd.setCursor(0,1);
    lcd.print(sent_params.current);
    lcd.setCursor(0,2);
    lcd.print(sent_params.fault);
    lcd.setCursor(0,3);
    lcd.print(sent_params.line_status);
    delay(1000);
}
/* ----------------------------------- EOF ---------------------------------- */