
#include "parking_monitor.h"
#include <Servo.h>

#define EXIT_MOTOR      4
#define ENTRANCE_MOTOR  7
#define CLOSED_POSITION 90
#define OPENED_POSITION 180
#define MOTOR_INTERVAL  25


Servo ExitGate;
Servo EntranceGate;

unsigned long exit_gateMillis;
unsigned long enter_gateMillis;

/// @brief Move a given motor to a specific position
/// @param exit the gate motor to be moved
/// @param close send true if the gate should be closed, false otherwise
/// @attention the motor is moved 1 step every MOTOR_INTERVAL period
void moveMotor(bool exit, bool close)
{
    if(exit)
    {
        if(millis() - exit_gateMillis < MOTOR_INTERVAL) return;
        exit_gateMillis = millis();

        int gate_position = ExitGate.read();

        if(close)
        {
            if(gate_position > CLOSED_POSITION)
            {
                ExitGate.write(gate_position - 1);
            }
        }
        else
        {
            if(gate_position < OPENED_POSITION)
            {
                ExitGate.write(gate_position + 1);
            }
        }
    }
    else
    {
        if(millis() - enter_gateMillis < MOTOR_INTERVAL) return;
        enter_gateMillis = millis();

        int gate_position = EntranceGate.read();

        if(close)
        {
            if(gate_position > CLOSED_POSITION)
            {
                EntranceGate.write(gate_position - 1);
            }
        }
        else
        {
            if(gate_position < OPENED_POSITION)
            {
                EntranceGate.write(gate_position + 1);
            }
        }
    }
}

/// @brief Close the exit gate
/// @retval GATE_CLOSED if the gate has been closed
///@retval GATE_MOVING if the gate is being closed
GATE_STATE gate_closeExit()
{
    if(ExitGate.read() <= CLOSED_POSITION) return GATE_CLOSED;
    moveMotor(true, true);
    
    return GATE_MOVING;
}

/// @brief Open the exit gate
/// @retval GATE_OPENED if the gate has been opened
///@retval GATE_MOVING if the gate is being opened
GATE_STATE gate_openExit()
{
    if(ExitGate.read() >= OPENED_POSITION) return GATE_OPENED;
    moveMotor(true, false);
    
    return GATE_MOVING;
}

/// @brief Open the entrance gate
/// @retval GATE_OPENED if the gate has been opened
///@retval GATE_MOVING if the gate is being opened
GATE_STATE gate_openEntrance()
{
    if(EntranceGate.read() >= OPENED_POSITION) return GATE_OPENED;
    moveMotor(false, false);
    
    return GATE_MOVING;
}

/// @brief Close the entrance gate
/// @retval GATE_CLOSED if the gate has been closed
///@retval GATE_MOVING if the gate is being closed
GATE_STATE gate_closeEntrance()
{
    if(EntranceGate.read() <= CLOSED_POSITION) return GATE_CLOSED;
    moveMotor(false, true);
    
    return GATE_MOVING;
}


bool gate_motorInit()
{
    EntranceGate.attach(ENTRANCE_MOTOR);
    ExitGate.attach(EXIT_MOTOR);

    return true;
}

/* ----------------------------------- EOF ---------------------------------- */