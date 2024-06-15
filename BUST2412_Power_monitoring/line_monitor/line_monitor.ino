
#include "line_monitor.h"
#define DATA_SEND_SECS  2
#define LINE_A1     10


LINE_PARAMS line_params;

unsigned long lastSentMillis;
unsigned long faultClearanceTime;
bool errorCodeSent;
bool waitFaultClearance;

void sendData()
{
    Serial.print("*");
    // Serial.print(line_data);
    Serial.print(line_params.voltage); Serial.print(",");
    Serial.print(line_params.current); Serial.print(",");
    Serial.print(line_params.fault); Serial.print(",");
    Serial.print(line_params.line_status); 
    Serial.println("#");

    lastSentMillis = millis();
}

void setLineFault(LINE_FAULT new_fault)
{
    if(waitFaultClearance) return;

    line_params.fault = new_fault;
    if(new_fault != NO_FAULT)
    {
        sendData();
    }
}

void turnLineOn( bool state )
{
    if(digitalRead(LINE_A1) == state)
        return;

    digitalWrite(LINE_A1, state);
    line_params.line_status = (uint8_t)state;
}

void setup()
{
    Serial.begin(9600);
    pinMode(LINE_A1, OUTPUT);
    turnLineOn(false);

    //testing
    // for(int i=0; i<5; i++)
    // {
    //     turnLineOn(true); delay(1000);
    //     turnLineOn(false); delay(1000);
    // }
    
    // device_displayInit();
    device_SensorInit();

    lastSentMillis = millis();
    errorCodeSent = true;
}

void loop()
{
    
    if(millis() - faultClearanceTime > 10000)
    {
        waitFaultClearance = false;
        turnLineOn(true);
        delay(500);
    }
    
    LINE_FAULT curr_fault = device_currentSensorRead();

    if(curr_fault != NO_FAULT)
    {
        turnLineOn(false);
        setLineFault(curr_fault);
    }

    LINE_FAULT volt_fault = device_voltageSensorRead();

    if(curr_fault == NO_FAULT)
    {
        setLineFault(volt_fault);
    }
    
    if(millis() - lastSentMillis > (DATA_SEND_SECS*1000))
    {
        sendData();
    }

    // device_displayUpdate();

    switch(line_params.fault)
    {
        case SHORT_CIRCUIT:
        case OVER_LOAD:
        case OVER_VOLTAGE:
        case UNDER_VOLTAGE:
            turnLineOn(false);
            if(!waitFaultClearance){
                waitFaultClearance = true;
                faultClearanceTime = millis();
            }
        break;

        case NO_FAULT:
        case OPEN_CIRCUIT:
            if(!waitFaultClearance){
                turnLineOn(true);
            }
        break;
    }
}