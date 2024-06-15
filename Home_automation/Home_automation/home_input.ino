

#include "home_config.h"
#include <ClickButton.h>

/** @brief Physical pin connectionof the button*/
#ifdef USING_NODEMCU
    #define BUTTON_PIN       13
#else
    #define BUTTON_PIN        3
#endif

/** @brief Debounce timer in ms */
#define  DEBOUNCE_TIME        20

/** @brief Time limit for multi clicks */
#define  MULTI_CLICK_TIME    500

/** @brief time until "held-down clicks" */
#define  LONG_CLICK_TIME    1500

ClickButton button(BUTTON_PIN, HIGH, CLICKBTN_PULLUP);

void light_inputSetup( void )
{
//    pinMode(BUTTON_PIN, FUNCTION_3);
    button.debounceTime   = DEBOUNCE_TIME;
    button.multiclickTime = MULTI_CLICK_TIME;
    button.longClickTime  = LONG_CLICK_TIME;
}

LIGHT_COMMAND light_inputRead(void)
{
    button.Update();
    if (button.clicks != 0)
    {
        switch (button.clicks)
        {
            case 1:
                #ifdef USING_NODEMCU
                    Serial.println("YOU CLICKED ONCE");
                #endif
                return LIGHT_SCREEN;
                break;
            case 2:
                #ifdef USING_NODEMCU
                    Serial.println("YOU CLICKED TWICE");
                #endif
                return SWITCH_LIGHT;
            break;
            case 3:
                #ifdef USING_NODEMCU
                    Serial.println("YOU CLICKED THRICE");
                #endif
                return SWITCH_SERVER;
            break;
            case -1:
                #ifdef USING_NODEMCU
                    Serial.println("YOU CLICKED LONGER 1"); 
                #endif
                return SWITCH_ALARM;
            break;
            case -3:
                #ifdef USING_NODEMCU
                    Serial.println("YOU CLICKED LONGER 2");
                #endif
                return FACTORY_RESET;
            break;
        }
    }
    return NO_COMMAND;
}
