
#include "gsm_home.h"
#include <Wire.h>
// #include <LiquidCrystal_PCF8574.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#define DISPLAY_INTERVAL    500
#define DISPLAY_ADDRESS    0X3C
#define SCREEN_WIDTH        128    // OLED display width, in pixels
#define SCREEN_HEIGHT        64    // OLED display height, in pixels
#define OLED_COLOR SH110X_WHITE

// LiquidCrystal_PCF8574 lcd(0x27);
Adafruit_SH1107 display = Adafruit_SH1107(SCREEN_HEIGHT, SCREEN_WIDTH, &Wire);

HOME_PARAMS display_params;
unsigned long display_millis;

void display_init(void)
{
    // Wire.begin();
    // Wire.beginTransmission(0x27);
    // int error = Wire.endTransmission();

    // lcd.begin(16, 2);
    // lcd.setBacklight(255);
    display.begin(DISPLAY_ADDRESS, true);
    display.setRotation(1);
    display_log("TECH CRUNCH");

    display_params.loadon = NO_CMD;
}

void display_log(char *log)
{
    display.clearDisplay();
    display.display();
    delay(1);

    display.setTextSize(2);
    display.setTextColor(OLED_COLOR);
    display.setCursor(0,0);
    display.println(log);
    display.display();
    delay(2000);
}


void display_update(void)
{
    if(millis() - display_millis < DISPLAY_INTERVAL) return;

    display_millis = millis();

    if(display_params.loadon != home_params.loadon)
    {
        display_params.loadon = home_params.loadon;
        display.clearDisplay();
        display.display();
        delay(1);

        

        display.setTextSize(2);
        display.setTextColor(OLED_COLOR);
        display.setCursor(31,16);
        display.println("LOAD");
        display.setCursor(47,38);
        if(home_params.loadon == LOAD_ON)
        {
            display.println("ON");
        }
        else
        {
            display.println("OFF");
        }
        display.display();
        
    }



}
