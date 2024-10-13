
#include "box_config.h"


#define BOX_RELAY       7
#define BOX_SENSOR      8
#define BOX_BUTTON      9
#define BOX_BUZZER      3
#define BUZZER_INTERVAL 1

typedef enum BOX_STATES
{
    BOX_STATE_INIT,
    BOX_STATE_UNLOCKED,
    BOX_STATE_UNLOCKING,
    BOX_STATE_LOCKED,
    BOX_STATE_BROKEN,
    BOX_STATE_ERROR
}
BOX_STATES;

const char BOX_IS_OPEN[] = "BOX IS OPENED";
const char BOX_IS_CLOSED[] = "BOX IS CLOSED";
const char SEND_SMS[] = "Sending sms...";

BOX_STATES box_main_state;
DRV_STATUS box_finger_sensor_status;
DRV_STATUS box_gsm_status;

unsigned long button_millis = 0;
uint8_t box_wait_count = 0;
uint8_t box_authorised_nb;
bool p1_authorised;
bool p2_authorised;
bool p3_authorised;
bool box_sms_sent;
bool box_btn_timer_started;

bool box_ButtonPressed( void )
{
    return !digitalRead(BOX_BUTTON);
}

bool box_IsDoorOpened( void )
{
    return digitalRead(BOX_SENSOR);
}

void box_OpenDoor( void )
{
    if(digitalRead(BOX_RELAY) == false)
    {
        digitalWrite(BOX_RELAY, HIGH);
    }
}

void box_CloseDoor( void )
{
    if(digitalRead(BOX_RELAY) != false)
    {
        digitalWrite(BOX_RELAY, LOW);
    }
}

bool box_SoundBuzzer( void )
{
    static unsigned long buzzer_millis = 0;
    if(millis() - buzzer_millis > BUZZER_INTERVAL*1500)
    {
        // tone(BOX_BUZZER, 4000, 150);
        buzzer_millis = millis();
        digitalWrite(BOX_BUZZER, HIGH);
        delay(200);
        digitalWrite(BOX_BUZZER, LOW);
    }
}

void box_SetMainState(  BOX_STATES new_state)
{
    if(box_main_state != new_state)
    {
        box_main_state = new_state;
        bdp_SetDisplayMsg("", 1);
        bdp_SetDisplayMsg("", 2);

        box_authorised_nb = 0;
        p1_authorised = false;
        p2_authorised = false;
        p3_authorised = false;
        box_sms_sent = false;

    }
}

void setup()
{
    Serial.begin(9600);

    pinMode(BOX_BUTTON, INPUT_PULLUP);
    pinMode(BOX_SENSOR, INPUT_PULLUP);
    pinMode(BOX_BUZZER, OUTPUT);
    pinMode(BOX_RELAY, OUTPUT);

    //lcd init
    if(bdp_InitDisplay() == DRV_STATUS_ERROR)
    {
        while(1);
    }

    bdp_DirectDisplay("Device Init");
    delay(2000);

    box_finger_sensor_status = bfp_InitSensor();
    if(box_finger_sensor_status == DRV_STATUS_ERROR)
    {
        bdp_SetDisplayMsg("SENSOR", 0);
        box_SetMainState(BOX_STATE_ERROR);
    }
    else if(box_finger_sensor_status == DRV_STATUS_WRONG)
    {
        bdp_SetDisplayMsg("ENROLL", 0);
        box_SetMainState(BOX_STATE_ERROR);
    }
    else
    {
        //gsm init
        bdp_DirectDisplay("Waiting GSM");
        if(gsm_InitModule() == DRV_STATUS_ERROR)
        {
            bdp_SetDisplayMsg("GSM COMM", 0);
            box_SetMainState(BOX_STATE_ERROR);
        }
        else
        {
            box_SetMainState(BOX_STATE_INIT);
        }
    }

    bdp_DirectDisplay("READY");
    delay(2000);
}

void loop()
{
    if(box_main_state != BOX_STATE_LOCKED)
    {
        box_authorised_nb = 0;
    }

    switch(box_main_state)
    {
        case BOX_STATE_INIT:
        {
            if(!box_IsDoorOpened())
            {
                box_CloseDoor();
                box_SetMainState(BOX_STATE_LOCKED);
            }
            else
            {
                //wait for door to close
                bdp_SetDisplayMsg(&BOX_IS_OPEN[0], 1);
            }
            break;
        }

        case BOX_STATE_LOCKED:
        {
            if(box_IsDoorOpened())
            {
                box_SetMainState(BOX_STATE_BROKEN);
            }
            bdp_SetDisplayMsg(&BOX_IS_CLOSED[0], 1);
            //read sms code
            if(box_sms_sent!= false)
            {
                if(box_btn_timer_started == false)
                {
                    bdp_SetDisplayMsg("Wait Code", 2);
                    button_millis = millis();
                    box_btn_timer_started = true;
                }

                DRV_STATUS gsm_status = gsm_StateMachineRun();

                if(gsm_status == DRV_STATUS_CORRECT)
                {
                    //UNLOCK
                    bdp_SetDisplayMsg("Authorised", 2);
                    box_OpenDoor();
                    box_SetMainState(BOX_STATE_UNLOCKING);
                }
                else if(gsm_status == DRV_STATUS_WRONG)
                {
                    bdp_SetDisplayMsg("Wrong Code", 2);
                    box_btn_timer_started = false;
                    box_sms_sent = true;
                }

                if(millis() - button_millis > 30000)
                {
                    box_btn_timer_started = false;
                    box_sms_sent = true;
                }
            }
            else if(box_ButtonPressed() != false)
            {
                if(box_btn_timer_started == false)
                {
                    button_millis = millis();
                    box_btn_timer_started = true;
                }

                if(millis() - button_millis > 10000)
                {
                    bdp_SetDisplayMsg("Sending Code...", 2);
                    gsm_SetMessage(1);
                    Serial.println("Code");
                    if(gsm_StateMachineRun() == DRV_STATUS_CORRECT)
                    {
                        box_SetMainState(BOX_STATE_LOCKED);
                        box_btn_timer_started = false;
                        box_sms_sent = true;
                    }
                }

            }
            else if(box_wait_count > 0)
            {
                box_wait_count--;
                delay(500);
            }
            else
            {
                box_btn_timer_started = false;
                switch(bfp_ReadSensor())
                {
                    case DRV_STATUS_NONE:
                    {
                        bdp_SetDisplayMsg("PUT FINGER", 2);
                        break;
                    }

                    case DRV_STATUS_WAITING:
                    {
                        bdp_SetDisplayMsg("ANALYSING", 2);
                        break;
                    }

                    case DRV_STATUS_FAILED:
                    {
                        bdp_SetDisplayMsg("TRY AGAIN", 2);
                        box_wait_count = 4;
                        break;
                    }

                    case DRV_STATUS_WRONG:
                    {
                        bdp_SetDisplayMsg("NOT AUTHORISED", 2);
                        box_wait_count = 4;
                        break;
                    }

                    case DRV_STATUS_CORRECT:
                    {
                        bdp_SetDisplayMsg("AUTHORISED", 2);
                        BOX_USERS auth_user = bfp_GetFingerPrintUser();
                        switch(auth_user)
                        {
                            case PERSON_1:
                            {
                                if(p1_authorised == false)
                                {
                                    p1_authorised = true;
                                    box_authorised_nb++;
                                }
                            }

                            case PERSON_2:
                            {
                                if(p2_authorised == false)
                                {
                                    p2_authorised = true;
                                    box_authorised_nb++;
                                }
                            }

                            case PERSON_3:
                            {
                                if(p3_authorised == false)
                                {
                                    p3_authorised = true;
                                    box_authorised_nb++;
                                }
                            }
                        }

                        box_wait_count = 4;

                        if(box_authorised_nb >= 3)
                        {
                            //unlock
                            box_OpenDoor();
                            box_SetMainState(BOX_STATE_UNLOCKING);
                        }

                        break;
                    }

                    case DRV_STATUS_ERROR:
                    {
                        box_SetMainState(BOX_STATE_ERROR);
                        break;
                    }
                }
            }

            break;
        }

        case BOX_STATE_UNLOCKING:
        {
            bdp_SetDisplayMsg(&BOX_IS_OPEN[0], 1);
            box_SoundBuzzer();
            if(box_IsDoorOpened())
            {
                // if(!box_sms_sent)
                // {
                //     box_sms_sent = true;
                //     gsm_SetMessage(2);
                //     bdp_SetDisplayMsg(&SEND_SMS[0], 2);
                // }
                // delay(2000);
                // if(gsm_StateMachineRun() == DRV_STATUS_CORRECT)
                // {
                    box_SetMainState(BOX_STATE_UNLOCKED);
                // }
            }
            break;
        }

        case BOX_STATE_UNLOCKED:
        {
            //send sms on close
            if(!box_IsDoorOpened())
            {
                bdp_SetDisplayMsg(&BOX_IS_CLOSED[0], 1);
                box_CloseDoor();
                delay(2000);
                if(!box_sms_sent)
                {
                    box_sms_sent = true;
                    gsm_SetMessage(3);
                    bdp_SetDisplayMsg(&SEND_SMS[0], 2);
                }
                if(gsm_StateMachineRun() == DRV_STATUS_CORRECT)
                {
                    box_SoundBuzzer();
                    box_SetMainState(BOX_STATE_LOCKED);
                }
            }
            else
            {
                //wait for door to close
                bdp_SetDisplayMsg(&BOX_IS_OPEN[0], 1);
            }
            break;
        }

        case BOX_STATE_BROKEN:
        {
            bdp_SetDisplayMsg("SAFE BROKEN", 1);
            bdp_SetDisplayMsg("ALARM ON", 2);
            box_SoundBuzzer();

            break;
        }

        case BOX_STATE_ERROR:
        {
            bdp_SetDisplayMsg("ERROR", 1);
            break;
        }

    }
    
    bdp_UpdateDisplay();
}


/* ----------------------------------- EOF ---------------------------------- */