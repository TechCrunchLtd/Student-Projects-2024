
#include "segregation_proto.h"
#include <Servo.h>

#define CLAMP_MOTOR         6
#define VERTICAL_MOTOR      5
#define ROTATIONAL_MOTOR    4
#define HORIZONTAL_MOTOR    2
#define MAXIMUM_HEIGHT      90
#define MINIMUM_HEIGHT      15
#define MAXIMUM_LENGTH      115
#define MINIMUM_LENGTH      85
#define MAXIMUM_WIDTH       178
#define MINIMUM_WIDTH       25
#define MAXIMUM_CLAMP       90
#define MINIMUM_CLAMP       50

Servo horizontalMotor;
Servo rotationalMotor;
Servo verticalMotor;
Servo clampMotor;

ROBOT_POSTION currentPosition;
ROBOT_LOCATION currentLocation;

ROBOT_LOCATION restPosition;
ROBOT_LOCATION infraredSensorLocation;
ROBOT_LOCATION metalSensorLocation;
ROBOT_LOCATION plasticSensorLocation;
ROBOT_LOCATION plasticBinLocation;
ROBOT_LOCATION paperBinLocation;
ROBOT_LOCATION metalBinLocation;
ROBOT_LOCATION generalBinLocation;


void setDeviceLocations(void)
{
    restPosition.horizontal = 90;
    restPosition.vertical = 90;
    restPosition.rotational = 90;
    
    infraredSensorLocation.horizontal = 112;
    infraredSensorLocation.vertical = MINIMUM_HEIGHT;
    infraredSensorLocation.rotational = 175;
    
    metalSensorLocation.horizontal = 112;
    metalSensorLocation.vertical = 30;
    metalSensorLocation.rotational = 140;
    
    plasticSensorLocation.horizontal = 112;
    plasticSensorLocation.vertical = 35;
    plasticSensorLocation.rotational = 120;
    
    plasticBinLocation.horizontal = 112;
    plasticBinLocation.vertical = 35;
    plasticBinLocation.rotational = 50;
    
    paperBinLocation.horizontal = 115;
    paperBinLocation.vertical = 35;
    paperBinLocation.rotational = 30;
    
    metalBinLocation.horizontal = 112;
    metalBinLocation.vertical = 35;
    metalBinLocation.rotational = 80;
    
    generalBinLocation.horizontal = 112;
    generalBinLocation.vertical = 35;
    generalBinLocation.rotational = 50;
}

void moveUp(void)
{
    if(currentLocation.vertical >= MAXIMUM_HEIGHT)
        return;

    currentLocation.vertical++;
    verticalMotor.write(currentLocation.vertical);

    currentLocation.vertical = verticalMotor.read();
}

void moveDown(void)
{
    if(currentLocation.vertical <= MINIMUM_HEIGHT)
        return;

    currentLocation.vertical--;
    verticalMotor.write(currentLocation.vertical);
    currentLocation.vertical = verticalMotor.read();
}

void moveBack(void)
{
    if(currentLocation.horizontal <= MINIMUM_LENGTH)
        return;

    currentLocation.horizontal--;
    horizontalMotor.write(currentLocation.horizontal);
    currentLocation.horizontal = horizontalMotor.read();
}

void moveFront(void)
{
    if(currentLocation.horizontal >= MAXIMUM_LENGTH)
        return;

    currentLocation.horizontal++;
    horizontalMotor.write(currentLocation.horizontal);
    currentLocation.horizontal = horizontalMotor.read();
}

void moveAntiClockwise(void)
{
    if(currentLocation.rotational >= MAXIMUM_WIDTH)
        return;

    currentLocation.rotational++;
    rotationalMotor.write(currentLocation.rotational);
    currentLocation.rotational = rotationalMotor.read();
}

void moveClockwise(void)
{
    if(currentLocation.rotational <= MINIMUM_WIDTH)
        return;

    currentLocation.rotational--;
    rotationalMotor.write(currentLocation.rotational);
    currentLocation.rotational = rotationalMotor.read();
}

uint8_t clampObject(void)
{
    uint8_t clamp_point = clampMotor.read();
    if(clamp_point > MINIMUM_CLAMP)
    {
        clamp_point -= 1;
        clampMotor.write(clamp_point);

    }

    return clamp_point;
}

uint8_t releaseObject(void)
{
    uint8_t clamp_point = clampMotor.read();

    if(clamp_point < MAXIMUM_CLAMP)
    {
        clamp_point += 1;
        clampMotor.write(clamp_point);
    }

    return clamp_point;
}

ROBOT_POSTION robotarm_initialise(void)
{
    setDeviceLocations();

    clampMotor.attach(CLAMP_MOTOR);
    verticalMotor.attach(VERTICAL_MOTOR);
    horizontalMotor.attach(HORIZONTAL_MOTOR);
    rotationalMotor.attach(ROTATIONAL_MOTOR);


    //read inital psotions of all the motors
    currentLocation.horizontal = horizontalMotor.read();
    currentLocation.vertical = verticalMotor.read();
    currentLocation.rotational = rotationalMotor.read();

    //move to the rest position
    while(robotarm_moveTo(REST_POSITION) != REST_POSITION)
    {
        delay(1000);
    }

    currentPosition = REST_POSITION;

    return REST_POSITION;
}

ROBOT_POSTION robotarm_moveTo(ROBOT_POSTION new_position)
{
    if(new_position == currentPosition)
        return currentPosition;
    
    ROBOT_LOCATION *new_location;
    
    switch(new_position)
    {
        case REST_POSITION:
            new_location = &restPosition;
        break;
        case INFRARED_SENSOR:
            new_location = &infraredSensorLocation;
        break;
        case METAL_SENSOR:
            new_location = &metalSensorLocation;
        break;
        case METAL_BIN:
            new_location = &metalBinLocation;
        break;
        case PAPER_BIN:
            new_location = &paperBinLocation;
        break;
        case PLASTIC_BIN:
            new_location = &plasticBinLocation;
        break;
        case PLASTIC_SENSOR:
            new_location = &plasticSensorLocation;
        break;
    }

    if((new_location->horizontal  == currentLocation.horizontal) &&
       (new_location->vertical  == currentLocation.vertical) &&
       (new_location->rotational  == currentLocation.rotational)
      )
    {
        currentPosition = new_position;
        display_log("moving... done");
        return currentPosition;
    }

    if(new_location->horizontal < currentLocation.horizontal) moveBack();
    else if(new_location->horizontal > currentLocation.horizontal) moveFront();

    if(new_location->vertical < currentLocation.vertical) moveDown();
    else if(new_location->vertical > currentLocation.vertical) moveUp();

    if(new_location->rotational < currentLocation.rotational) moveClockwise();
    else if(new_location->rotational > currentLocation.rotational) moveAntiClockwise();
    
    if(device_params.device_state == STATE_DUMP_WASTE)
    {
        char buf[17] = {0};
        sprintf(buf, "Pos%d: %d,%d,%d", new_position, new_location->horizontal, new_location->vertical, new_location->rotational);
        display_log(buf);
    }
    return IN_MOTION;
}

DEVICE_RESULT robotarm_liftObject(void)
{
    //clamp object
    if(clampObject() > MINIMUM_CLAMP)
        return DEVICE_FALSE;

    // //move UP
    moveUp();
    if(currentLocation.vertical >= MAXIMUM_HEIGHT/2)
    {
        return DEVICE_TRUE;
    }

    return DEVICE_TRUE;
}

DEVICE_RESULT robotarm_dropObject(void)
{
    //move down
    if(currentLocation.vertical > MINIMUM_HEIGHT)
    {
        moveDown();
        return DEVICE_FALSE;
    }

    //unclamp object
    if(releaseObject() >= MAXIMUM_CLAMP)
        return DEVICE_TRUE;

    return DEVICE_FALSE;
}

/* ----------------------------------- EOF ---------------------------------- */