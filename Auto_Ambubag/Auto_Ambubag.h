

#ifndef AUTO_AMBUBAG_H
    #define AUTO_AMBUBAG_H

    // #define DEVICE_DEBUG

    /** @brief - Defines the maximum allowed string length for any given variable in the program */
    #define DEVICE_MAX_STR_LEN    16

    #define BUTTON      2

    typedef enum DEVICE_ERROR
    {
        NO_ERROR,
        INPUT_ERROR,
        OUTPUT_ERROR,
        DISPLAY_ERROR

    } DEVICE_ERROR;

    typedef enum MOTOR_STATE
    {
        MOTOR_IDLE,
        MOTOR_PRESSING,
        MOTOR_RELEASING
    } MOTOR_STATE;

    typedef struct DEVICE_PARAMS
    {
        DEVICE_ERROR device_error;
        char error_message[DEVICE_MAX_STR_LEN];
        bool motor_running;
        bool device_sleeping;
        uint8_t sleep_time;
        uint8_t motor_rate;

    } DEVICE_PARAMS;

    extern DEVICE_PARAMS device_params;

    bool input_setup(void (*inputCallback)(void));
    void input_update();

    bool display_setup();

    void display_clear();
    void display_update( bool on_error);


    void motor_setUp();
    void motor_stop();
    bool motor_run();

#endif

/* ----------------------------------- EOF ---------------------------------- */