
#include "parking_monitor.h"

#define BUZZER 12
#define BUZZER_RATE_SECS 1
#define NOTIFICATION_INTERVAL_SECS  1

PARKING_PARAMS parking_params;

void resetParams()
{
    parking_params.exit_gate = GATE_UNKNOWN;
    parking_params.entrance_gate = GATE_UNKNOWN;
    for(int slot=0; slot<TOTAL_PARKING_SLOTS; slot++)
        parking_params.parking_slots[slot] = SLOT_FREE;
    
}

void pushNotification()
{
    String status = "*";
    for(int sensor=0; sensor<TOTAL_PARKING_SLOTS; sensor++)
    {
        status += String(parking_params.parking_slots[sensor]);
        if(sensor+1 == TOTAL_PARKING_SLOTS) break;
        status += ",";
    }
    status = status.substring(0,-1);
    status += "#";

    Serial.println(status);
}

uint8_t countFreeSlots()
{
    uint8_t count = 0;
    for(int sensor=0; sensor<TOTAL_PARKING_SLOTS; sensor++)
    {
        if(parking_params.parking_slots[sensor] == false)
        {
            count++;
        }
    }

    return count;
}

void beepBuzzer()
{
    static unsigned long buzzer_millis = 0;
    if(millis() - buzzer_millis > BUZZER_RATE_SECS*1000)
    {
        // tone(BUZZER, 4000, 150);
        analogWrite(BUZZER, 180);
        delay(60);
        analogWrite(BUZZER, 0);
        buzzer_millis = millis();
    }
}

unsigned long notificationMillis;
PARKING_PARAMS gateCommand;

void testMotor()
{
    unsigned long myTime = 0;
    bool openit = false;
    while(1)
    {
        if(millis() - myTime > 10000) {
            openit = !openit;
            myTime = millis();
        }

        if(openit) gate_openEntrance();
        else       gate_closeEntrance();

        delay(1);
    }
}

void setup()
{
    Serial.begin(9600);

    pinMode(BUZZER, OUTPUT);

    resetParams();

    slot_sensorInit();

    gate_motorInit();

    while (parking_params.entrance_gate != GATE_CLOSED)
    {
        parking_params.entrance_gate = gate_closeEntrance();
    }

    while (parking_params.exit_gate != GATE_CLOSED)
    {
        parking_params.exit_gate = gate_closeExit();
    }
    // testMotor();
}


void loop()
{
    //Read slot sensors
    slot_sensorRead();

    #ifdef DEBUG_PARKING
        Serial.print("Entsta: ");
        Serial.println(parking_params.entrance_gate);
        Serial.print("Extsta: ");
        Serial.println(parking_params.exit_gate);
    #endif

    //handle entrance gate
    switch (parking_params.entrance_gate)
    {
        case GATE_CLOSED:
            if(gate_checkEntranceRequest() == true && countFreeSlots() > 0)
            {
                gateCommand.entrance_gate = GATE_OPENED;
                parking_params.entrance_gate = gate_openEntrance();
            }
        break;

        case GATE_OPENED:
            if(gate_checkEntranceRequest() == false)
            {
                gateCommand.entrance_gate = GATE_CLOSED;
                parking_params.entrance_gate = gate_closeEntrance();
            }
        break;

        case GATE_MOVING:
            beepBuzzer();
            if(gateCommand.entrance_gate == GATE_CLOSED)
            {
                if(gate_closeEntrance() == GATE_CLOSED)
                {
                    parking_params.entrance_gate = GATE_CLOSED;
                }
            }
            else if(gateCommand.entrance_gate == GATE_OPENED)
            {
                if(gate_openEntrance() == GATE_OPENED)
                {
                    parking_params.entrance_gate = GATE_OPENED;
                }
            }
        break;
    
        default:
            if(gate_closeEntrance() == GATE_CLOSED)
            {
                parking_params.entrance_gate = GATE_CLOSED;
            }
        break;
    }

    //handle exit gate
    switch (parking_params.exit_gate)
    {
        case GATE_CLOSED:
            if(gate_checkExitRequest() == true)
            {
                gateCommand.exit_gate = GATE_OPENED;
                parking_params.exit_gate = gate_openExit();
            }
        break;

        case GATE_OPENED:
            if(gate_checkExitRequest() == false)
            {
                gateCommand.exit_gate = GATE_CLOSED;
                parking_params.exit_gate = gate_closeExit();
            }
        break;

        case GATE_MOVING:
            beepBuzzer();
            if(gateCommand.exit_gate == GATE_CLOSED)
            {
                if(gate_closeExit() == GATE_CLOSED)
                {
                    parking_params.exit_gate = GATE_CLOSED;
                }
            }
            else if(gateCommand.exit_gate == GATE_OPENED)
            {
                if(gate_openExit() == GATE_OPENED)
                {
                    parking_params.exit_gate = GATE_OPENED;
                }
            }
        break;
    
        default:
            if(gate_closeExit() == GATE_CLOSED)
            {
                parking_params.exit_gate = GATE_CLOSED;
            }
        break;
    }


    //send notification when time is due
    if(millis() - notificationMillis > NOTIFICATION_INTERVAL_SECS*1000)
    {
        pushNotification();
        notificationMillis = millis();
    }
}

/* ----------------------------------- EOF ---------------------------------- */