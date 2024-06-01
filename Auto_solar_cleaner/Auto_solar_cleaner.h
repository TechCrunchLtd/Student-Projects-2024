
#ifndef AUTO_SOLAR_CLEANER_H
    #define AUTO_SOLAR_CLEANER_H

    typedef struct SOLAR_PARAMS
    {
        float current;
        float voltage;
        uint16_t light;
        bool panel_is_clean;
    }
    SOLAR_PARAMS;

    extern SOLAR_PARAMS solar_params;

    /// @brief  setup system driver to control movement 
    /// @return true
    bool driver_initialise(void);

    /// @brief     Move the system forward 
    /// @return    true if the system has reached the end point
    /// @attention movement automatically stops when end point is reached
    bool driver_moveForward(void);

    /// @brief     Move the system backwards
    /// @return    true if the system has reached the start point
    /// @attention movement automatically stops when start point is reached
    bool driver_moveBackward(void);

    /// @brief   Stop system movement immediately
    /// @return Return false is system stopped along the way
    bool driver_moveStop(void);

    /// @brief  setup panel monitor to monitor power inline with light intensity 
    /// @return true
    bool monitor_initialise(void);

    /// @brief     Check if the panel needs cleaning
    /// @attention This function runs in a statewise manner.
    ///            It will first determine the power being output by the panel
    ///            that's by reading the voltage and current sensor.
    ///            It then determines the expected power by reading the LDR.
    /// @return    true if the expected power is inline with the power being received, thus no cleaning is required.
    bool monitor_panelClean(void);

    /// @brief  Initialise the panel cleaner peripheral 
    /// @return true if all accessories are fitted
    bool cleaner_initialise(void);

    /// @brief   Turns on the cleaner motor/brush and water/cleaning solution.
    /// @attention  cleaning will continue until the stopcleaning function is invoked.
    /// @return  false if the cleaning is not happening as expected or can completely not go on
    bool cleaner_startCleaning(void);

    /// @brief      Stop the cleaning process.
    /// @return     false if cleaning has stopped, true otherwise.
    bool cleaner_stopCleaning(void);

#endif