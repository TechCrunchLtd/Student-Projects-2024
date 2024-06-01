/**
 * @file driver.ino
 * @brief This files has all the functionality of the system movement
 * @details It specifically manages the movement by conrtrolling the two wheel motors
*/

#include "Auto_solar_cleaner.h"

#define START_POSITION_SENSOR   A5
#define STOP_POSITION_SENSOR   A4
#define FRONT_MOTOR_PIN_1       2
#define FRONT_MOTOR_PIN_2       4
#define BACK_MOTOR_PIN_1       5
#define BACK_MOTOR_PIN_2       6
#define DISTANCE_TIME_mS        5000

bool forward_movement;
bool backward_movement;

unsigned long distance_timer;

void stopMovement(void)
{
    //stop motor movement
    digitalWrite(FRONT_MOTOR_PIN_1, HIGH);
    digitalWrite(FRONT_MOTOR_PIN_2, LOW);
    digitalWrite(BACK_MOTOR_PIN_1, HIGH);
    digitalWrite(BACK_MOTOR_PIN_2, LOW);

    forward_movement = false;
    backward_movement = false;
}

bool timeElapsed(void)
{
    if(millis() - distance_timer > DISTANCE_TIME_mS)
    {
        return true;
    }

    return false;
}

/// @brief determine if the system has moved to end point by reading the sensor mounted above
/// @return true if the sensor has moved beyond the panel notifying the end point
bool reachedEndPoint(void)
{
    //infrared snesor is active low
    // if(digitalRead(STOP_POSITION_SENSOR) == LOW)
    // {
    //     return true;
    // }

    return timeElapsed();
}

/// @brief determine if the system has moved to starting point by reading the sensor mounted above
/// @return true if the sensor has moved beyond the panel notifying the end point
bool reachedStartPoint(void)
{
    //infrared snesor is active low
    // if(digitalRead(START_POSITION_SENSOR) == LOW)
    // {
    //     return true;
    // }

    return timeElapsed();
}

bool driver_initialise(void)
{
    //setup movement motor
    pinMode(BACK_MOTOR_PIN_1, OUTPUT);
    pinMode(BACK_MOTOR_PIN_2, OUTPUT);
    pinMode(FRONT_MOTOR_PIN_1, OUTPUT);
    pinMode(FRONT_MOTOR_PIN_2, OUTPUT);

    //setup position sensors

    stopMovement();

    return true;
}

bool driver_moveForward(void)
{
    //move forward
    if(forward_movement == false)
    {
        digitalWrite(FRONT_MOTOR_PIN_1, LOW);
        digitalWrite(FRONT_MOTOR_PIN_2, LOW);
        digitalWrite(BACK_MOTOR_PIN_1, LOW);
        digitalWrite(BACK_MOTOR_PIN_2, LOW);

        forward_movement = true;
        backward_movement = false;
        distance_timer = millis();
    }
    else if(reachedEndPoint() == true)
    {
        stopMovement();
        return true;
    }

    return false;
}

bool driver_moveBackward(void)
{
    //move backwards
    if(backward_movement == false)
    {
        digitalWrite(FRONT_MOTOR_PIN_1, HIGH);
        digitalWrite(FRONT_MOTOR_PIN_2, HIGH);
        digitalWrite(BACK_MOTOR_PIN_1, HIGH);
        digitalWrite(BACK_MOTOR_PIN_2, HIGH);

        backward_movement = true;
        forward_movement = false;
        distance_timer = millis();
    }
    else if(reachedStartPoint() == true)
    {
        stopMovement();
        return true;
    }
    return false;
}

/* ----------------------------------- EOF ---------------------------------- */