
#include "device_config.h"

#define DEVICE_UPDATE_PERIOD 1000
#define BUZZER D5
#define BUTTON D0

unsigned long device_updateMillis;
bool message_sent = false;

DEVICE_PARAMS device_params;

void soundBuzzer()
{
    static unsigned long buzzer_time = 0;
    if(millis() - buzzer_time > 1000)
    {
        tone(BUZZER, 4000, 100);
        buzzer_time = millis();
    }
}

void processLineErrors()
{
    static uint8_t previous_fault = 0;
    char fault_message[DEVICE_MAX_STR_LEN*2] = {0};

    if(device_params.line_fault != previous_fault)
    {
        previous_fault = device_params.line_fault;
        message_sent = false;
    }

    memset(device_params.error_message, '\0', strlen(device_params.error_message));

    switch(device_params.line_fault)
    {
        case 0:
            message_sent = true;
        break;

        case 1: //short circuit
            sprintf(device_params.error_message, "Short circuit");
        break;

        case 2: //overload
            sprintf(device_params.error_message, "Overload");
        break;

        case 3: //under voltage
            sprintf(device_params.error_message, "Under voltage");
        break;

        case 4: //over voltage
            sprintf(device_params.error_message, "Over voltage");
        break;

        case 5: //open circuit
            sprintf(device_params.error_message, "Open Circuit");
        break;
    }

    if(!message_sent)
    {

        #ifdef DEVICE_DEBUG
            Serial.println(device_params.error_message);
            Serial.print("Ft: "); Serial.println(device_params.line_fault);
        #endif
        sprintf(fault_message, "%s occured at A01. Line %s.",
                device_params.error_message,
                device_params.line_status?"on":"off");
        device_gsmSendSMS(fault_message);
        message_sent = true;

        device_serverLogFault(fault_message);
    }
}


void device_errorHandler()
{
    switch (device_params.device_error)
    {
        case GSM_ERROR:
            sprintf(device_params.error_message, "GSM failure!");
        break;
        case COMM_ERROR:
            sprintf(device_params.error_message, "Communication failure!");
        break;
    
        case FILE_ERROR:
            device_displayError("Filesystem error!");
        break;

        case LINE_ERROR:
            processLineErrors();
            soundBuzzer();
        break;
    
        case NO_ERROR:
            memset(device_params.error_message, '\0', DEVICE_MAX_STR_LEN);
            device_params.device_error = NO_ERROR;
            message_sent = false;
        break;
    
    default:
        break;
    }

}

bool device_setError( DEVICE_ERROR error)
{
    if((device_params.device_error != NO_ERROR) && (error != NO_ERROR))
        return false;

    device_params.device_error = error;
    
#ifdef DEVICE_DEBUG
    Serial.print("E::");
    Serial.println(device_params.device_error);
#endif

    return true;
}

/// @brief first things first
void setup()
{
    Serial.begin(9600);
    delay(100);
#ifdef DEVICE_DEBUG
    Serial.println("Starting...");
#endif

    pinMode(BUZZER, OUTPUT);
    device_displayInit();

    if(!device_serverInit()){
        device_errorHandler();
    }
    
    //CONFIGURATION MODE 
    if(digitalRead(BUTTON) == LOW)
    {

        if(!device_serverStart()){
            device_errorHandler();
        }
        while(1)
        {
            device_serverUpdate();
            device_displayUpdate(true);
            delay(1);
        }
    }
    // if(!device_inputSetup()){
    //     device_errorHandler();
    // }
    
    
    if(!device_espnowInit()){
        device_errorHandler();
    }
    
    if(!device_gsmInit()){
        device_errorHandler();
    }

    for(int i=0; i<3; i++){
        tone(BUZZER, 4000, 100);
        delay(1000);
    }
    
#ifdef DEVICE_DEBUG
    Serial.println("Set up complete");
#endif
}

/// @brief keep it going
void loop()
{
    if (millis() - device_updateMillis > DEVICE_UPDATE_PERIOD)
    {
        device_displayUpdate(false);
        device_updateMillis = millis();
    }

    if(device_espnowUpdate() == false)
    {
        device_setError(COMM_ERROR);
    }

    device_errorHandler();

}


/* ----------------------------------- EOF ---------------------------------- */
