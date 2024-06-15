
#ifndef WATER_MONITOR_H
    #define WATER_MONITOR_H

    #define DEBUG_WATER_MONITOR

    typedef struct WATER_PARAMS
    {
        uint8_t temperature;
        float ph_level;
        uint8_t depth;
    }
    WATER_PARAMS;

    extern WATER_PARAMS water_params;

    void display_init(void);
    void display_update(void);

    void sensors_init(void);
    void sensors_readValues(void);

#endif