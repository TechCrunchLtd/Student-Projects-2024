
#ifndef PARKING_MONITOR_H
    #define PARKING_MONITOR_H

    #define DEBUG_PARKING
    #define TOTAL_PARKING_SLOTS   2

    typedef enum PARKING_STATE
    {
        SLOT_FREE,
        SLOT_OCCUPIED
    }
    PARKING_STATE;
    
    typedef enum GATE_STATE
    {
        GATE_UNKNOWN,
        GATE_CLOSED,
        GATE_OPENED,
        GATE_MOVING,
    }
    GATE_STATE;

    typedef struct PARKING_PARAMS
    {
        PARKING_STATE parking_slots[TOTAL_PARKING_SLOTS];
        GATE_STATE entrance_gate;
        GATE_STATE exit_gate;
        bool buzzer_state;

    }PARKING_PARAMS;

    extern PARKING_PARAMS parking_params;

    void slot_sensorInit();
    void slot_sensorRead();
    
    bool gate_checkExitRequest();
    bool gate_checkEntranceRequest();

    bool gate_motorInit();
    GATE_STATE gate_closeExit();
    GATE_STATE gate_openExit();
    GATE_STATE gate_openEntrance();
    GATE_STATE gate_closeEntrance();
#endif