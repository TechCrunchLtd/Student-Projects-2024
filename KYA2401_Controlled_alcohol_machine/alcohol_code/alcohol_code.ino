
#include "alchol_code.h"
#define BUZZER      6
#define INDICATOR   13
#define ALCOHOL_SENSOR  2
#define OPERATOR_LINE   A2
#define CONTROL_LINE    A0

DEVICE_PARAMS device_params;

void alertOn(bool buzzer)
{
    digitalWrite(INDICATOR, HIGH);
    if(buzzer)
        digitalWrite(BUZZER, HIGH);
}

void alertOff(void)
{
    digitalWrite(BUZZER, LOW);
    digitalWrite(INDICATOR, LOW);
}

void setup()
{
    #ifdef DEBUG_DEVICE
    Serial.begin(9600);
    #endif
    
    pinMode(BUZZER, OUTPUT);
    pinMode(INDICATOR, OUTPUT);
    pinMode(ALCOHOL_SENSOR, INPUT);
    pinMode(OPERATOR_LINE, INPUT);
    pinMode(CONTROL_LINE, INPUT);

    DISP_initialise();

    while(digitalRead(OPERATOR_LINE) == LOW)
    {
        sprintf(device_params.log_msg, "ERROR: SENSOR IN NON INIT STATE");
        // sprintf(device_params.log_msg, "ERROR: MACHINE   NON INIT STATE");
        DISP_update();
        delay(200);
    }

    machineInit();

    device_params.device_state = STATE_IDLE;
    memset(device_params.log_msg, '\0', MAX_STR_LEN);
    #ifdef DEBUG_DEVICE
    Serial.println("Init Completed");
    #endif
}


void loop()
{
    switch(device_params.device_state)
    {
        case STATE_IDLE:
            if(digitalRead(OPERATOR_LINE) == LOW)
            {
                if(digitalRead(ALCOHOL_SENSOR) == LOW)
                {
                    device_params.device_state = STATE_NORMAL;
                }
                else
                {
                    device_params.device_state = STATE_ALCOHOL;
                }
            }
            machineStop();

            if(digitalRead(CONTROL_LINE) == HIGH)
            {
                sprintf(device_params.log_msg, "ERROR: OPERATOR NOT CHECKED!");
                alertOn(0);
            }
            else
            {
                sprintf(device_params.log_msg, "WAITING CHECKIN BY OPERATOR");
                alertOff();
            }
        break;

        case STATE_NORMAL:
            if(digitalRead(CONTROL_LINE) == HIGH)
            {
                machineRun();
            }
            else
            {
                machineStop();
            }
            alertOff();

            if(digitalRead(ALCOHOL_SENSOR) == HIGH)
            {
                device_params.device_state = STATE_ALCOHOL;
            }
        break;

        case STATE_ALCOHOL:
            machineStop();
            alertOn(1);
            if(digitalRead(CONTROL_LINE) == LOW)
            {
                sprintf(device_params.log_msg, "ALCOHOL DETECTED OPERATOR FLAGED");
            }
            else
            {
                sprintf(device_params.log_msg, "ALCOHOL DETECTED OPERATOR DENIED");
            }
        break;
    }

    DISP_update();
    memset(device_params.log_msg, '\0', MAX_STR_LEN);

    if(digitalRead(OPERATOR_LINE) == HIGH)
    {
        device_params.device_state = STATE_IDLE;
    }

    delay(10);
}

/* ----------------------------------- EOF ---------------------------------- */