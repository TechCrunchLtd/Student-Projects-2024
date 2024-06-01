
#ifndef GSM_HOME_H
    #define GSM_HOME_H
    typedef enum HOME_CMD
    {
        NO_CMD,
        LOAD_OFF,
        LOAD_ON,
        LOAD_TOGGLE
    }
    HOME_CMD;

    typedef struct HOME_PARAMS
    {
        HOME_CMD loadon;

    }
    HOME_PARAMS;

    extern HOME_PARAMS home_params;

    void display_init(void);
    void display_update(void);
    void display_log(char *log);

    void gsm_init(void);
    HOME_CMD gsm_run(void);

#endif