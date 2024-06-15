/**
 * @file patient_config.h
 * @defgroup home_files
*/

#ifndef AUTO_HOME_H
    #define AUTO_HOME_H

    #include <Wire.h>
    #include <LittleFS.h>
    #include <Arduino_JSON.h>

    /** @brief - When debugging using the serial monitor and nodemcu */
    // #define DEVICE_DEBUG
    #define DEBUG_TASKS


    /** @brief - Defines the maximum allowed string length for any given variable in the program */
    #define DEVICE_MAX_STR_LEN    25

    typedef enum SENSOR_STATUS
    {
        SENSOR_ERROR,
        SENSOR_READY,
        SENSOR_NO_FINGER,
        SENSOR_BUSY,
        SENSOR_VALUES_READY
    }SENSOR_STATUS;

    /** @brief - Defines all the different types of errors to expect*/
    typedef enum DEVICE_ERROR
    {
        /** @brief - No errors so far */
        NO_ERROR,

        /** @brief - Errors to do with the light commands, for example, when a commnad execution fails */
        CMD_ERROR,

        /**@brief - Errors to do with file handling, for example, if a file fails to open or can't be written */
        FILE_ERROR,

        /** @brief - Time errors, for example, failure to communcate or to retrieve time */
        TIME_ERROR,

        /** @brief - Input errors, for example, failure to communcate or to retrieve sensor values */
        INPUT_ERROR,

        /** @brief - Communication errors, for example, failure to connect to WiFi or online service */
        COMM_ERROR
    }
    DEVICE_ERROR;

    /** @brief - Defines all the parameters that the light works with be it volatile or non-volatile parameters*/
    typedef struct DEVICE_PARAMS
    {
        char device_id[DEVICE_MAX_STR_LEN];
        char wifi_name[DEVICE_MAX_STR_LEN];
        char wifi_password[DEVICE_MAX_STR_LEN];
        char current_date[DEVICE_MAX_STR_LEN];
        char current_time[DEVICE_MAX_STR_LEN];
        char local_server[DEVICE_MAX_STR_LEN];
        char error_message[DEVICE_MAX_STR_LEN];

        /** @brief - holds the current error yet to be cleared */
        DEVICE_ERROR device_error;

        /** @brief - holds the current state of the server */
        bool server_running;

        /** @brief - when this value is false, online updates of the device values won't be pushed */
        bool updates_enabled;

        /** @brief - holds how long the light should stay activated beofre automatically stopping*/
        uint16_t update_interval;

        /** @brief - holds the current value of the last read temperature */
        uint8_t temperature;

        /** @brief - holds the current value of the last read oxygen level */
        uint8_t oxygen_level;

        /** @brief - holds the current value of the last read heart rate */
        uint8_t heart_rate;
    }
    DEVICE_PARAMS;

    extern DEVICE_PARAMS device_params;
     

    /* ---------------------- Lets prototype all functions ---------------------- */
    
    /// @brief This function should be called to in case of any error
    /// @param new_error the excat type of error that has occured
    /// @note The next state will be determined by the type  of error submitted
    void device_errorHandler( DEVICE_ERROR new_error);

    /// @brief setup the peripherals to be used as primary inpput
    bool device_inputSetup( void );

    /// @brief get light commands from the primary input device
    /// @return the specific commandas processed by the input device
    bool device_inputRead(void);

    /// @brief Checks if the display is available and boots it up
    /// @return false if the display can not be found
    bool device_displayInit( void );

    /// @brief update the display with the given information
    void device_displayUpdate( void );

    /// @brief Display a given error message
    /// @param error is a pointer the error message string which is to be displayed
    /// @attention This function does not return, the code will stay in this function until an external reset occurs
    void device_displayError( char *error );

    /// @brief Initialise the time keeper and ensure it's running normally
    /// @return false in case of failure to communicate with the time keeper
    bool device_timeInit( void );

    /// @brief Reads and gets the current time and date from the time keeper
    /// @return false if the current time can not be determined, this can be due to failure to communicate with the time keeper
    bool device_timeGet( void );

    /// @brief updates the time keeper with the current date and time
    /// @param dt a string containing both date and time
    /// @note The param should be formated as this "Dec 03 2023 16:35:56"
    bool device_timeSet(String dts);

    /// @brief Get the current day of the week from the time manager 
    /// @return current hour betwwen 0 and 6
    uint8_t device_timeGetDay( void );
    
    /// @brief Get the current hour from the time manager 
    /// @return current hour betwwen 0 and 23
    uint8_t device_timeGetHour( void );
    
    /// @brief Get the current mins that have passe on a given hour from the time manager 
    /// @return current mins betwwen 0 and 59
    uint8_t device_timeGetMin( void );

    /// @brief Reads the light configured states and initilalises the device parameters from memory apppropriately
    /// @note This function will always start the online service by calling the device_onlineInit function
    bool device_serverInit( void );

    /// @brief This function will turn on the server 
    /// @note This function won't return unless it successfully connects to WiFi network/hotspot
    /// @retval True if the server starts successfully
    bool device_serverStart( void );

    /// @brief This will turn off the server 
    /// @retval false if the server has been stopped
    bool device_serverStop( void );

    /// @brief This function saves the cureent light configurations to memory
    /// @note This should be called after the intended configurations have been set.
    /// @retval true if the saving was successfull
    bool device_saveConfigurations( void );

    /// @brief Rstore back the default values/configurations 
    void device_ResetDefaults( void );

    /// @brief get any inputs coming in from the server side
    /// @return false is the server update didn't complete properly
    bool device_serverUpdate( void );

    /// @brief Initialise the online service
    /// @note In most cases this will be MQTT or CAOP
    bool device_onlineInit( void );

    /// @brief Send a given message to the online platform
    /// @param res_msg A string containing the message to be sent online.
    bool device_onlineSend(String res_msg);

    /// @brief This function takes care of all the online business.
    /// @return true if the was a handled request from online service
    /// @note This function does not handle local server requests but online those from online service
    bool device_onlineRequestHandler( void );

#endif

/* ------------------------------- END OF FILE ------------------------------ */
