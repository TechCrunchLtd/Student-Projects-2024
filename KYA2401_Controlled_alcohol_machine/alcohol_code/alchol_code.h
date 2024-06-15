
#ifndef ALCOHOL_COD_H
    #define ALCOHOL_COD_H
    #define MAX_STR_LEN 32
    // #define DEBUG_DEVICE

    typedef enum DEVICE_STATE
    {
        STATE_IDLE,
        STATE_NORMAL,
        STATE_ALCOHOL,
    }
    DEVICE_STATE;

    typedef struct DEVICE_PARAMS
    {
        DEVICE_STATE device_state;
        bool machine_running;
        bool machine_direction;
        char log_msg[MAX_STR_LEN];
    }
    DEVICE_PARAMS;

    extern DEVICE_PARAMS device_params;

    bool DISP_initialise();
    void DISP_update( void);

    void machineInit(void);
    void machineRun(void);
    void machineStop(void);

#endif