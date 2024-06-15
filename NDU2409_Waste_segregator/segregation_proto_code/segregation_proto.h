

#ifndef SEGREGATION_PROTO_H
    #define SEGREGATION_PROTO_H

    // #define DEBUG_DEVICE

    typedef enum DEVICE_STATE
    {
        STATE_ERROR = -1,
        STATE_REST,
        STATE_ENTER_CHECK,
        STATE_CHECK_WASTE,
        STATE_DUMP_WASTE,
        STATE_ENTER_REST
    }
    DEVICE_STATE;

    typedef enum DEVICE_RESULT
    {
        /** @brief - Returned when a given device or peripheral is in error state*/
        DEVICE_ERROR = -1,

        /** @brief - Valid response form the device*/
        DEVICE_FALSE,

        /** @brief - Valid response form the device*/
        DEVICE_TRUE
    }
    DEVICE_RESULT;

    typedef enum DEVICE_WASTE_TYPE
    {
        NO_WASTE,
        PAPER_WASTE,
        METALLIC_WASTE,
        PLASTIC_WASTE,

        /** @brief - This could be organic waste or something else that the system can't detect*/
        OTHER_WASTE
    }
    DEVICE_WASTE_TYPE;

    typedef enum ROBOT_POSTION
    {
        IN_MOTION,
        REST_POSITION,
        INFRARED_SENSOR,
        METAL_SENSOR,
        PLASTIC_SENSOR,
        METAL_BIN,
        PLASTIC_BIN,
        PAPER_BIN,
        GENERAL_BIN
    }
    ROBOT_POSTION;

    typedef struct ROBOT_LOCATION
    {
        uint8_t horizontal;
        uint8_t vertical;
        uint8_t rotational;
    }
    ROBOT_LOCATION;

    typedef struct DEVICE_PARAMS
    {
        DEVICE_STATE device_state;
        ROBOT_POSTION current_position;
        DEVICE_WASTE_TYPE current_waste;

    }DEVICE_PARAMS;

    extern DEVICE_PARAMS device_params;

    DEVICE_RESULT display_initialise(void);
    DEVICE_RESULT display_update(void);
    void display_log(char *msg);

    DEVICE_RESULT sensor_initialise(void);
    DEVICE_RESULT sensor_readInfrared(void);
    DEVICE_RESULT sensor_checkIfPaper(void);
    DEVICE_RESULT sensor_checkIfMetallic(void);
    DEVICE_RESULT sensor_checkIfPlastic(void);

    DEVICE_RESULT robotarm_liftObject(void);
    DEVICE_RESULT robotarm_dropObject(void);
    ROBOT_POSTION robotarm_initialise(void);
    ROBOT_POSTION robotarm_moveTo(ROBOT_POSTION new_position);
    void mapBoardPositions(void);

#endif 