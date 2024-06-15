
#include "patient_config.h"

#define DEVICE_UPDATE_PERIOD 1000

unsigned long device_updateMillis;

#ifdef DEBUG_TASKS
    unsigned long taskMillis;
#endif

DEVICE_PARAMS device_params;

void device_errorHandler(DEVICE_ERROR new_error)
{
#ifdef DEVICE_DEBUG
    Serial.println(new_error);
#endif
    
    if(new_error == COMM_ERROR)
        return;

    switch (new_error)
    {
    case CMD_ERROR:
        sprintf(device_params.error_message, "Command failure!");
        break;
    
    case FILE_ERROR:
        device_displayError(strcat(device_params.error_message, "\nPlease foramt the filesystem and/or re-upload device files!"));
        break;

    case INPUT_ERROR:
        device_displayError(device_params.error_message);
        break;
    
    case NO_ERROR:
        memset(device_params.error_message, '\0', DEVICE_MAX_STR_LEN);
        device_params.device_error = NO_ERROR;
        break;
    
    default:
        break;
    }

}

/// @brief first things first
void setup()
{
#ifdef DEVICE_DEBUG || #ifdef DEBUG_TASKS
    Serial.begin(115200);
    delay(100);
    Serial.println("Starting...");
#endif

    device_displayInit();

    if(!device_inputSetup()){
        device_errorHandler(device_params.device_error);
    }
    device_timeInit();
    
    if(!device_serverInit()){
        device_errorHandler(device_params.device_error);
    }
    
#ifdef DEVICE_DEBUG
    Serial.println("Set up complete");
#endif
}

/// @brief keep it going
void loop()
{
    #ifdef DEBUG_TASKS
        taskMillis = millis();
    #endif

    if (millis() - device_updateMillis > DEVICE_UPDATE_PERIOD)
    {
        // device_inputRead();
        device_timeGet();
        device_displayUpdate();
        device_updateMillis = millis();
    }
    #ifdef DEBUG_TASKS
        Serial.print("dp: ");
        Serial.println(millis() - taskMillis);
        taskMillis = millis();
    #endif

    device_inputRead();
    #ifdef DEBUG_TASKS
        Serial.print("Ip: ");
        Serial.println(millis() - taskMillis);
        taskMillis = millis();
    #endif

    device_serverUpdate();
    #ifdef DEBUG_TASKS
        Serial.print("Sv: ");
        Serial.println(millis() - taskMillis);
        taskMillis = millis();
    #endif
    device_onlineRequestHandler();
    #ifdef DEBUG_TASKS
        Serial.print("ol: ");
        Serial.println(millis() - taskMillis);
        taskMillis = millis();
    #endif

}


/* ----------------------------------- EOF ---------------------------------- */