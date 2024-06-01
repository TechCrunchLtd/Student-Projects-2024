
#ifndef PROJECT_HEADER_H
    #define PROJECT_HEADER_H
    #define SIMULATION_CODE
    #define MAX_STR_LEN     16
    #define SENSOR_SAMPLES  10

    typedef struct DEVICE_PARAMS
    {
        bool pump_on;
        bool raining;
        float temperature;
        uint8_t humidity;
        uint8_t soil_moisture;
        char log_msg[MAX_STR_LEN];
    }
    DEVICE_PARAMS;

    extern DEVICE_PARAMS device_params;
    /// @brief  Get the display ready 
    /// @retval - false if the initialisation process fails
    bool display_initialise();

    /// @brief Update the display with the current 
    /// @param log setting this parameter to true will cause the display to log the message in the log_msg buffer of the light_params
    void display_update(bool log);

    bool sensor_Init();
    void sensor_ReadValues();

#endif