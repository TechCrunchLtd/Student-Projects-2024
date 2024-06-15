
#include "segregation_proto.h"

#define BUZZER              3
#define BUZZER_DURATION     100

DEVICE_PARAMS device_params;

void soundBuzzer(uint16_t interval)
{
    static unsigned long buzzer_time = 0;
    if(millis() - buzzer_time < interval) return;
    buzzer_time = millis();

    if(interval > BUZZER_DURATION)
    tone(BUZZER, 4000, BUZZER_DURATION);
}


void mapBoardPositions(void)
{
    ROBOT_POSTION map_points = REST_POSITION;
    while(1)
    {
        int i = 0;
        switch(map_points)
        {
            case REST_POSITION:
                if(robotarm_moveTo(REST_POSITION) == REST_POSITION)
                {
                    for(i=0; i<10; i++){
                        soundBuzzer(1000);
                        delay(500);
                        display_log("REST P");
                    }

                    map_points = INFRARED_SENSOR;
                }
            break;

            case INFRARED_SENSOR:
                if(robotarm_moveTo(INFRARED_SENSOR) == INFRARED_SENSOR)
                {
                    // while(1)
                    for(i=0; i<10; i++)
                    {
                        soundBuzzer(1000);
                        delay(500);
                        display_log("RED SS");
                        // if(robotarm_liftObject() == DEVICE_TRUE)
                        // {
                        //     break;
                        // }
                    }
                    map_points = METAL_SENSOR;
                }
            break;

            case METAL_SENSOR:
                if(robotarm_moveTo(METAL_SENSOR) ==METAL_SENSOR)
                {
                    for(i=0; i<10; i++){
                        soundBuzzer(1000);
                        delay(500);
                        display_log("METAL SS");
                    }

                    map_points = PLASTIC_SENSOR;
                }
            break;

            case PLASTIC_SENSOR:
                if(robotarm_moveTo(PLASTIC_SENSOR) ==PLASTIC_SENSOR)
                {
                    for(i=0; i<10; i++){
                        soundBuzzer(1000);
                        delay(500);
                        display_log("PLASTIC SS");
                    }
                    map_points = METAL_BIN;
                }
            break;

            case METAL_BIN:
                if(robotarm_moveTo(METAL_BIN) ==METAL_BIN)
                {
                    for(i=0; i<10; i++){
                        soundBuzzer(1000);
                        delay(500);
                        display_log("METAL BIN");
                    }
                    map_points = PAPER_BIN;
                }
            break;

            case PAPER_BIN:
                if(robotarm_moveTo(PAPER_BIN) ==PAPER_BIN)
                {
                    for(i=0; i<10; i++){
                        soundBuzzer(1000);
                        delay(500);
                        display_log("PAPER BIN");
                    }
                    map_points = PLASTIC_BIN;
                }
            break;

            case PLASTIC_BIN:
                if(robotarm_moveTo(PLASTIC_BIN) == PLASTIC_BIN)
                {
                    // while(1)
                    for(i=0; i<10; i++)
                    {
                        soundBuzzer(1000);
                        delay(500);
                        display_log("PLASTIC BIN");
                        // if(robotarm_dropObject() == DEVICE_TRUE)
                        // {
                        //     break;
                        // }
                    }
                    // map_points = GENERAL_BIN;
                    map_points = REST_POSITION;
                }
            break;

            case GENERAL_BIN:
                if(robotarm_moveTo(GENERAL_BIN) == GENERAL_BIN)
                {
                    for(i=0; i<10; i++){
                        soundBuzzer(1000);
                        delay(500);
                        display_log("GEN BIN");
                    }
                    map_points = REST_POSITION;
                }
            break;
        }

        delay(60);
    }
}


DEVICE_RESULT dumpWasteToBin(DEVICE_WASTE_TYPE waste_type)
{
    static bool drop_waste = false;

    if(drop_waste == true)
    {
        soundBuzzer(300);
        if(robotarm_dropObject() == DEVICE_TRUE)
        {
            drop_waste = false;
            return DEVICE_TRUE;
        }

        return DEVICE_FALSE;
    }

    //using paper instead of general bin
    ROBOT_POSTION waste_bin = PAPER_BIN;
    switch(waste_type)
    {
        case NO_WASTE:
        case OTHER_WASTE:
            waste_bin = PAPER_BIN;
        break;

        case PAPER_WASTE:
            waste_bin = PAPER_BIN;
        break;

        case METALLIC_WASTE:
            waste_bin = METAL_BIN;
        break;

        case PLASTIC_WASTE:
            waste_bin = PLASTIC_BIN;
        break;
    }

    if(robotarm_moveTo(waste_bin) == waste_bin)
    {
        device_params.current_position = waste_bin;
        drop_waste = true;
    }

    return DEVICE_FALSE;
}


void setup()
{
    #ifdef DEBUG_DEVICE
    Serial.begin(115200);
    #endif

    pinMode(BUZZER, OUTPUT);
    #ifdef DEBUG_DEVICE
    Serial.println("Di");
    #endif
    display_initialise();

    display_log("Sen Init..."); delay(1000);
    
    #ifdef DEBUG_DEVICE
    Serial.println("Si");
    #endif
    while(!sensor_initialise())
    {
        delay(1000);
        soundBuzzer(1500);
    }

    display_log("R t..."); delay(1000);
    #ifdef DEBUG_DEVICE
    Serial.println("R_i");
    #endif
    while(robotarm_initialise() != REST_POSITION)
    {
        delay(10);
        soundBuzzer(1500);
    }

    // mapBoardPositions();

    device_params.current_position = REST_POSITION;
    device_params.device_state = STATE_REST;
    device_params.current_waste = NO_WASTE;
    

    #ifdef DEBUG_DEVICE
    Serial.println("Init!");
    #endif
}


void loop()
{
    switch(device_params.device_state)
    {
        case STATE_ENTER_REST:
            //goto rest position
            if(robotarm_moveTo(REST_POSITION) == REST_POSITION)
            {
                device_params.current_position = REST_POSITION;
                device_params.current_waste = NO_WASTE;
                device_params.device_state = STATE_REST;
            }
        break;

        case STATE_REST:
            //wait for ir sensor to trigger
            if(sensor_readInfrared() == DEVICE_TRUE)
            {
                soundBuzzer(1000);
                device_params.current_position = REST_POSITION;
                device_params.current_waste = NO_WASTE;
                device_params.device_state = STATE_ENTER_CHECK;
                #ifdef DEBUG_DEVICE
                Serial.println("new w");
                #endif
            }

        break;

        case STATE_ENTER_CHECK:
            if(robotarm_moveTo(INFRARED_SENSOR) == INFRARED_SENSOR)
            {
                soundBuzzer(1000);
                device_params.current_position = INFRARED_SENSOR;
                // device_params.device_state = STATE_CHECK_WASTE;
                #ifdef DEBUG_DEVICE
                Serial.println("g-cw");
                #endif
                delay(1000);
                if(robotarm_dropObject() == DEVICE_TRUE)
                {
                    soundBuzzer(1000);
                    device_params.device_state = STATE_CHECK_WASTE;
                    device_params.current_waste = NO_WASTE;
                }
            }
        break;

        case STATE_CHECK_WASTE:
            //check which waste has come in
            DEVICE_RESULT sensor_result = DEVICE_FALSE;

            switch(device_params.current_waste)
            {
                case NO_WASTE:
                    //start checking
                    if(robotarm_liftObject() == DEVICE_TRUE)
                    {
                        soundBuzzer(1000);
                        device_params.current_waste = PAPER_WASTE;
                        device_params.device_state = STATE_CHECK_WASTE;
                        delay(1000);
                    }

                break;

                case PAPER_WASTE:
                    //check if waste is paper
                    sensor_result = sensor_checkIfPaper();
                    
                    if(sensor_result == DEVICE_TRUE)
                    {
                        device_params.device_state = STATE_DUMP_WASTE;
                        delay(1000);
                    }
                    else if(sensor_result == DEVICE_FALSE)
                    {
                        device_params.current_waste = METALLIC_WASTE;
                        delay(1000);
                    }
                    if(sensor_result == DEVICE_ERROR)
                    {
                        device_params.device_state = STATE_ERROR;
                    }

                break;

                case METALLIC_WASTE:
                    //check if waste is metallic
                    if(robotarm_moveTo(METAL_SENSOR) == METAL_SENSOR)
                    {
                        //fisrt ensure you have reached the target sensor
                        soundBuzzer(1000);
                        device_params.current_position = METAL_SENSOR;

                        //get the sensor reading
                        sensor_result = sensor_checkIfMetallic();
                        if(sensor_result == DEVICE_TRUE)
                        {
                            device_params.device_state = STATE_DUMP_WASTE;
                            delay(1000);
                        }
                        else if(sensor_result == DEVICE_FALSE)
                        {
                            device_params.current_waste = PLASTIC_WASTE;
                            delay(1000);
                        }
                        if(sensor_result == DEVICE_ERROR)
                        {
                            device_params.device_state = STATE_ERROR;
                        }
                    }
                break;

                case PLASTIC_WASTE:
                    //check if waste is plastic
                    if(robotarm_moveTo(PLASTIC_SENSOR) == PLASTIC_SENSOR)
                    {
                        soundBuzzer(1000);
                        //fisrt ensure you have reached the target sensor
                        device_params.current_position = PLASTIC_SENSOR;

                        //get the sensor reading
                        sensor_result = sensor_checkIfPlastic();
                        if(sensor_result == DEVICE_TRUE)
                        {
                            device_params.device_state = STATE_DUMP_WASTE;
                            delay(1000);
                        }
                        else if(sensor_result == DEVICE_FALSE)
                        {
                            device_params.current_waste = OTHER_WASTE;
                            delay(1000);
                        }
                        if(sensor_result == DEVICE_ERROR)
                        {
                            device_params.device_state = STATE_ERROR;
                        }
                    }
                break;

                case OTHER_WASTE:
                    device_params.device_state = STATE_DUMP_WASTE;
                    delay(1000);
                break;
            }
            #ifdef DEBUG_DEVICE
            Serial.print("ws: ");
            Serial.print(device_params.current_waste);
            Serial.print("ss: ");
            Serial.println(sensor_result);
            #endif
        break;

        case STATE_DUMP_WASTE:
            //dumb waste in the right bin
            // soundBuzzer(400);
            // DEVICE_RESULT waste_dumped = dumpWasteToBin(device_params.current_waste);
            // if(waste_dumped == DEVICE_TRUE)
            // {
            //     soundBuzzer(1000);
            //     device_params.device_state = STATE_ENTER_REST;
            // }

        break;

        case STATE_ERROR:
            //alert for error
            soundBuzzer(500);
        break;
    }
    
    display_update();
    if(device_params.device_state == STATE_DUMP_WASTE)
    {
        soundBuzzer(1000);
        DEVICE_RESULT waste_dumped = dumpWasteToBin(device_params.current_waste);
        if(waste_dumped == DEVICE_TRUE)
        {
            soundBuzzer(1000);
            device_params.device_state = STATE_ENTER_REST;
        }
    }
    delay(30);

}