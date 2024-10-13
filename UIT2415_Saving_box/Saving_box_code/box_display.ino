
#include "box_config.h"
#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>

#define DISP_INTERVAL   1000
#define DISP_LENGTH     17
char empty_line[] = "                ";
typedef struct DISP_MSG
{
    char line1[DISP_LENGTH];
    char line2[DISP_LENGTH];
}
DISP_MSG;

LiquidCrystal_PCF8574 lcd(0x27);
DISP_MSG prev_msg;
DISP_MSG curr_msg;

DRV_STATUS bdp_InitDisplay( void )
{
    Wire.begin();
    Wire.beginTransmission(0x27);
    int error = Wire.endTransmission();
    if(error == 0)
    {
        lcd.begin(16, 2);
        lcd.setBacklight(255);
        lcd.home();
    }
    else
    {
        Serial.println("No LCD");
        return DRV_STATUS_ERROR;
    }

    return DRV_STATUS_CORRECT;
}

DRV_STATUS bdp_UpdateDisplay( void )
{
    static unsigned long disp_millis = 0;
    if(millis() - disp_millis > DISP_INTERVAL)
    {
        if(strcmp(prev_msg.line1, curr_msg.line1) != 0)
        {
            lcd.setCursor(0,0);
            lcd.print(empty_line);
            lcd.setCursor(0,0);
            lcd.print(curr_msg.line1);
            // sprintf(prev_msg.line1, "%s", curr_msg.line1);
            strcpy(prev_msg.line1, curr_msg.line1);
        }

        if(strcmp(prev_msg.line2, curr_msg.line2) != 0)
        {
            lcd.setCursor(0,1);
            lcd.print(empty_line);
            lcd.setCursor(0,1);
            lcd.print(curr_msg.line2);
            // sprintf(prev_msg.line1, "%s", curr_msg.line1);
            strcpy(prev_msg.line2, curr_msg.line2);
        }

        disp_millis = millis();
    }
}

DRV_STATUS bdp_DirectDisplay( char *msg )
{
    lcd.clear();
    lcd.print(msg);
}

DRV_STATUS bdp_SetDisplayMsg( const char *msg, uint8_t line_nb)
{
    switch(line_nb)
    {
        case 0: //error msg
        {
            memset(curr_msg.line1, '\0', DISP_LENGTH);
            memset(curr_msg.line2, '\0', DISP_LENGTH);
            sprintf(curr_msg.line1, "%s", msg);
            sprintf(curr_msg.line2, "ERROR");
            break;
        }

        case 1: //line 1
        {
            memset(curr_msg.line1, '\0', DISP_LENGTH);
            sprintf(curr_msg.line1, "%s", msg);
            break;
        }

        case 2: //line 2
        {
            memset(curr_msg.line2, '\0', DISP_LENGTH);
            sprintf(curr_msg.line2, "%s", msg);
            break;
        }
    }
}

/* ----------------------------------- EOF ---------------------------------- */