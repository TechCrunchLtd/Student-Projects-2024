
#ifndef AWS_CONFIG_H
    #define AWS_CONFIG_H

    #define DEBUG_AWS_DEVICE

    typedef struct AWS_PARAMS
    {
        /** @brief stroes the value of the current temperature of water in degrees celcius*/
        uint8_t temperature;

        /** @brief stroes the value of the current water level in cm*/
        uint8_t water_level;

        /** @brief stroes the value of the current humidity level as a percentage*/
        uint8_t humuidity;

        /** @brief Stroes the current state of the water pump*/
        bool pumpOn;

        /** @brief Stroes the current state of the peltier module*/
        bool peltierOn;
    }
    AWS_PARAMS;

    extern AWS_PARAMS AWS_params;


    /// @brief Configures the sensors initial state
    /// @retval false is sensors can't be configured
    bool sensors_setup();

    /// @brief reads the current sensors status
    /// @retval false if any of the sensors is unreadable
    bool sensors_read();

    /// @brief set up the display and ensure its attached
    /// @retval false if the display can't be found
    bool display_setup();

    /// @brief update the display with current status
    void display_update();
#endif
