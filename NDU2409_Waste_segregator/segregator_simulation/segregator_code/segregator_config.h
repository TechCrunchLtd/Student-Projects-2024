

#ifndef SEGREGATOR_CONFIG_H
    #define SEGREGATOR_CONFIG_H

    #define DEVICE_DEBUG
    #define SIMULATION

    typedef enum WASTE_TYPE
    {
        WASTE_NONE,
        WASTE_UNDEFINED,
        WASTE_METALLIC,
        WASTE_PLASTIC,
        WASTE_ORGANIC
    } WASTE_TYPE;

    typedef struct DEVICE_PARAMS
    {
        WASTE_TYPE waste_detected;
    }
    DEVICE_PARAMS;

    extern DEVICE_PARAMS device_params;

    WASTE_TYPE waste_detectType();

    bool waste_detectionSetup();
    bool waste_checkPresence();

    bool display_init();

    void display_update();

#endif