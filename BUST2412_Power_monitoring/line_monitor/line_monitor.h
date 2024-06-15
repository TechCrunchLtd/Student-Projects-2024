
#ifndef LINE_MONITOR_H
    #define LINE_MONITOR_H

    typedef enum LINE_FAULT
    {
        NO_FAULT,
        SHORT_CIRCUIT,
        OVER_LOAD,
        UNDER_VOLTAGE,
        OVER_VOLTAGE,
        OPEN_CIRCUIT
    } LINE_FAULT;

    typedef struct LINE_PARAMS
    {
        uint16_t voltage;
        uint16_t current;
        uint8_t fault;
        uint8_t line_status;
    } LINE_PARAMS;

    extern LINE_PARAMS line_params;

    /// @brief Checks if the display is available and boots it up
    /// @return false if the display can not be found
    bool device_displayInit( void );

    /// @brief update the display with the given information
    void device_displayUpdate( void );

    void device_displayNotify(char *msg);

    /// @brief Initialises the voltage sensor
    /// @return false if sensor isnt avalible
    bool device_SensorInit();

    /// @brief reads current sensor to determine possible faults
    /// @retval NO_FAULT
    // @retval  SHORT_CIRCUIT - Inrush current of high magnitude is detected, probably due to live and nuetral bonding.
    // @retval  OVER_LOAD - if the curretn line loading exceeds the desig capcity
    LINE_FAULT device_currentSensorRead();

    /// @brief Reads the voltage sensor for possible faults
    /// @retval NO_FAULT - Wheen all looks good
    /// @retval UNDER_VOLTAGE - When voltage is below the required threshold
    /// @retval OVER_VOLTAGE - When voltage is above the required threshold
    /// @retval OPEN_CICUIT - When the line seems energized but the load recives no power
    LINE_FAULT device_voltageSensorRead();


#endif
