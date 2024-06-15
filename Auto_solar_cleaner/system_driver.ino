/**
 * @file driver.ino
 * @brief This files has all the functionality of the system movement
 * @details It specifically manages the movement by conrtrolling the two wheel motors
*/

#include "Auto_solar_cleaner.h"

#define START_POSITION_SENSOR   A5
#define STOP_POSITION_SENSOR   A4
#define MOTOR_PIN               6
#define MOTOR_SPEED             255
// #define BACK_MOTOR_PIN_1       5
#define DISTANCE_TIME_mS        10000

bool forward_movement;
bool backward_movement;

unsigned long distance_timer;

void stopMovement(void)
{
    digitalWrite(MOTOR_PIN, LOW);
    
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

    // return timeElapsed();
    return true;
}

bool driver_initialise(void)
{
    //setup movement motor
    pinMode(MOTOR_PIN, OUTPUT);

    //setup position sensors

    stopMovement();

    return true;
}

bool driver_moveForward(void)
{
    //move forward
    if(forward_movement == false)
    {
        analogWrite(MOTOR_PIN, MOTOR_SPEED);

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
   /* if(backward_movement == false)
    {
        digitalWrite(FRONT_MOTOR_PIN_1, HIGH);
        digitalWrite(FRONT_MOTOR_PIN_2, HIGH);

        backward_movement = true;
        forward_movement = false;
        distance_timer = millis();
    }
    else if(reachedStartPoint() == true)
    {
        stopMovement();
        return true;
    }*/
    return true;
}

/* ----------------------------------- EOF ---------------------------------- */