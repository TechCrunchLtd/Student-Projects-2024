
/// @brief pin on which light one is connected
#define LIGHT_ONE       9

/// @brief pin on which light two is connected
#define LIGHT_TWO       10

/// @brief pin on which light three is connected
#define LIGHT_THREE     11


/// @brief pin on which sensor for light one is connected
#define SENSOR_ONE      5

/// @brief pin on which sensor for light two is connected
#define SENSOR_TWO      6

/// @brief pin on which sensor for light three is connected
#define SENSOR_THREE    7

/// @brief Set how bright the lights should be when motion is sensed
/// @details The value ranges between 0 and 255, where 0 is fully off, and 255 is fully on
#define LIGHT_ON_BRIGHTNESS    255

/// @brief Set how bright the lights should be when dimmed
/// @details The value ranges between 0 and 255, where 0 is fully off, and 255 is fully on
#define LIGHT_DIM_BRIGHTNESS    100

#define MOTION_DETECTED true
#define LIGHT_ON        true
#define LIGHT_OFF       false

// @brief Comment this line to disable debugging messages in serail
#define DEBUG_VIA_SERAIL

bool lightOne_state = true;
bool lightTwo_state = true;
bool lightThree_state = true;

/// @brief This function reads the current state of a given sensor
/// @param sensor_pin SENSOR_ONE or SENSOR_TWO or SENSOR_THREE
/// @return True if the sensor has sensed presensce/motion and false otherwise
bool readSensor( uint8_t sensor_pin )
{
    bool sensor_on = false;
    if(digitalRead(sensor_pin) == false)
    {
        sensor_on = true;

        #ifdef DEBUG_VIA_SERAIL
            Serial.print("Sensor ");
            switch(sensor_pin)
            {
                case SENSOR_ONE:
                    Serial.print("one");
                break;
                case SENSOR_TWO:
                    Serial.print("two");
                break;
                case SENSOR_THREE:
                    Serial.print("three");
                break;
            }
            Serial.println(" on");
        #endif
        
    }

    return sensor_on;
}

/// @brief This function sets a given light to a given state
/// @param light The light you want to switch, that is; LIGHT_ONE, LIGHT_TWO or LIGHT_THREE
/// @param new_state The state you want the light to be set to.
void switchLightState( uint8_t light, bool new_state )
{
    switch(light)
    {
        case LIGHT_ONE:
            if( new_state !=lightOne_state )
            {
                lightOne_state = new_state;
                if(new_state == LIGHT_OFF)
                {
                    analogWrite(LIGHT_ONE, LIGHT_DIM_BRIGHTNESS);
                }
                else
                {
                    analogWrite(LIGHT_ONE, LIGHT_ON_BRIGHTNESS);
                }

                #ifdef DEBUG_VIA_SERAIL
                    Serial.println("Light one toggled");
                #endif
            }

        break;

        case LIGHT_TWO:
            if( new_state !=lightTwo_state )
            {
                lightTwo_state = new_state;
                if(new_state == LIGHT_OFF)
                {
                    analogWrite(LIGHT_TWO, LIGHT_DIM_BRIGHTNESS);
                }
                else
                {
                    analogWrite(LIGHT_TWO, LIGHT_ON_BRIGHTNESS);
                }

                #ifdef DEBUG_VIA_SERAIL
                    Serial.println("Light two toggled");
                #endif
            }
        break;

        case LIGHT_THREE:
            if( new_state !=lightThree_state )
            {
                lightThree_state = new_state;
                if(new_state == LIGHT_OFF)
                {
                    analogWrite(LIGHT_THREE, LIGHT_DIM_BRIGHTNESS);
                }
                else
                {
                    analogWrite(LIGHT_THREE, LIGHT_ON_BRIGHTNESS);
                }

                #ifdef DEBUG_VIA_SERAIL
                    Serial.println("Light three toggled");
                #endif
            }
        break;
    }
}

void setup()
{
    #ifdef DEBUG_VIA_SERAIL
        Serial.begin(9600);
    #endif

    pinMode(LIGHT_ONE, OUTPUT);
    pinMode(LIGHT_TWO, OUTPUT);
    pinMode(LIGHT_THREE, OUTPUT);

    switchLightState(LIGHT_ONE, LIGHT_OFF);
    switchLightState(LIGHT_TWO, LIGHT_OFF);
    switchLightState(LIGHT_THREE, LIGHT_OFF);

    #ifdef DEBUG_VIA_SERAIL
        Serial.println("Init complete");
    #endif
}

void loop()
{
    //check sensor one
    if(readSensor(SENSOR_ONE) == MOTION_DETECTED)
    {
        switchLightState(LIGHT_ONE, LIGHT_ON);
    }
    else
    {
        switchLightState(LIGHT_ONE, LIGHT_OFF);
    }

    //check sensor two
    if(readSensor(SENSOR_TWO) == MOTION_DETECTED)
    {
        switchLightState(LIGHT_TWO, LIGHT_ON);
    }
    else
    {
        switchLightState(LIGHT_TWO, LIGHT_OFF);
    }
    //check sensor three
    if(readSensor(SENSOR_THREE) == MOTION_DETECTED)
    {
        switchLightState(LIGHT_THREE, LIGHT_ON);
    }
    else
    {
        switchLightState(LIGHT_THREE, LIGHT_OFF);
    }
}