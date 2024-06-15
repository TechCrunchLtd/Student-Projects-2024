
#include "Auto_Ambubag.h"
#include <Servo.h>

#define MOTOR_PIN 9
#define MOTOR_START_POINT   0
#define MOTOR_STOP_POINT    180

Servo motor;
MOTOR_STATE motorState;

uint16_t motorPosition;

void motor_setUp()
{
    motor.attach(MOTOR_PIN);

    #ifdef DEVICE_DEBUG
        Serial.println("Motor ready");
    #endif

    motorPosition = motor.read();
    motorPosition++;

    do{
        //reset motor to zero point
        motor.write(motorPosition - 1);
        motorPosition = motor.read();
    }
    while(motorPosition != MOTOR_START_POINT);

    motorState = MOTOR_IDLE;
}

bool motor_run()
{
    static unsigned long run_millis = 0;

    if((millis() - run_millis < device_params.motor_rate) || (device_params.motor_rate == 0)) return;
    run_millis = millis();

    motorPosition = motor.read();
    if(motorPosition == MOTOR_START_POINT)
    {
        motorState = MOTOR_PRESSING;
    }
    else if(motorPosition == MOTOR_STOP_POINT)
    {
        motorState = MOTOR_RELEASING;
    }

    switch (motorState)
    {
        case MOTOR_RELEASING:
            if(motorPosition != MOTOR_START_POINT){
                motor.write(motorPosition - 1);
            }
        break;

        case MOTOR_PRESSING:
            if(motorPosition != MOTOR_STOP_POINT){
                motor.write(motorPosition + 1);
            }
        break;

        case MOTOR_IDLE:
            motorState = MOTOR_PRESSING;
        break;
        
        default:
            motorState = MOTOR_IDLE;
        break;
    }
}

void motor_stop()
{
    motorPosition = motor.read();
    if(motorPosition == MOTOR_START_POINT)
    {
        motorState = MOTOR_IDLE;
    }
    else
    {
        motor.write(motorPosition - 1);
    }

}

/* ----------------------------------- EOF ---------------------------------- */