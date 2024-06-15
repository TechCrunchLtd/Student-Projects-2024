/**
 * @author @haboya
 * @file home_config.h
 * @defgroup home_files
*/

#ifndef AUTO_HOME_H
    #define AUTO_HOME_H

    #include <Wire.h>
    #include <LittleFS.h>
    #include <Arduino_JSON.h>

    /** @brief - When debugging using the serial monitor and nodemcu */
    // #define USING_NODEMCU

    /** @brief - Defines the number of characters occupied by a single alarm string */
    #define LIGHT_ALARM_SIZE      6

    /** @brief - Defines the maximum allowed string length for any given variable in the program */
    #define LIGHT_MAX_STR_LEN    21

    /** @brief - Defiens the maximum number of alarms that can be handled */
    #define LIGHT_MAX_ALARMS     30

    /** @brief - Defines all the possible light command/operations*/
    typedef enum LIGHT_COMMAND
    {
        /** @brief - This command will cause the light data to be erased back to the default values */
        FACTORY_RESET = -2,

        /** @brief - This command will cuase the function light_errorHandler to be executed */
        ERROR_COMMAND,
        /** @brief - No Command received yet */
        NO_COMMAND,

        LIGHT_SCREEN,

        /** @brief - This command will cause the periodic alarms to be turned to the opposite state */
        SWITCH_ALARM,

        /** @brief - This command turns off the server if it's on and vice-versa */
        SWITCH_SERVER,

        /** @brief - This commands triggers a fresh popualte of the curent days alarms from the server/storage*/
        GET_ALARMS,

        /** @brief - This command will switch the light on or off*/
        SWITCH_LIGHT
    }
    LIGHT_COMMAND;

    /** @brief - Defines all the different types of errors to expect*/
    typedef enum LIGHT_ERROR
    {
        /** @brief - Errors to do with the light commands, for example, when a commnad execution fails */
        CMD_ERROR,

        /**@brief - Errors to do with file handling, for example, if a file fails to open or can't be written */
        FILE_ERROR,

        /** @brief - RTC errors, for example, failure to communcate or to retrieve time*/
        RTC_ERROR
    }
    LIGHT_ERROR;

    /** @brief - Defines the values of a typical light alarm */
    typedef struct LIGHT_ALARM
    {
        /** @brief - holds the hours value of a given alarm */
        uint8_t hrs;

        /** @brief - holds the minutes value of a given alarm */
        uint8_t mins;
    }
    LIGHT_ALARM;

    /** @brief - Defines all the parameters that the light works with be it volatile or non-volatile parameters*/
    typedef struct LIGHT_PARAMS
    {
        char whatsapp_number[LIGHT_MAX_STR_LEN];
        char wifi_password[LIGHT_MAX_STR_LEN];
        char wifi_name[LIGHT_MAX_STR_LEN];
        char current_date[LIGHT_MAX_STR_LEN];
        char current_time[LIGHT_MAX_STR_LEN];
        char server_address[LIGHT_MAX_STR_LEN];

        /** @brief - holds the next alarm the light will activate */
        char next_alarm[LIGHT_ALARM_SIZE];

        /** @brief - holds the current state of the server */
        bool server_running;

        /** @brief - holds the current state of the light alarms */
        bool light_alarm_on;

        /** @brief - This value is true if the light is currently activated */
        bool light_activated;

        /** @brief - holds how long the light should stay activated beofre automatically stopping*/
        uint16_t light_duration;

        uint16_t update_interval;
    }
    LIGHT_PARAMS;

    extern LIGHT_PARAMS light_params;
     

    /* ---------------------- Lets prototype all functions ---------------------- */
    
    /// @brief This function should be called to in case of any error
    /// @param new_error the excat type of error that has occured
    /// @note The next state will be determined by the type  of error submitted
    void light_errorHandler( LIGHT_ERROR new_error);

    /// @brief setup the peripherals to be used as primary inpput
    void light_inputSetup( void );

    /// @brief get light commands from the primary input device
    /// @return the specific commandas processed by the input device
    /// @retval ONCE button was presed once
    /// @retval TWICE button was presed tow times
    LIGHT_COMMAND light_inputRead(void);

    /// @brief Checks if the display is available and boots it up
    /// @return false if the display can not be found
    bool light_displayInit( void );

    /// @brief update the display with the given information
    void light_displayUpdate( void );

    /// @brief This function resets the display sleep timer
    /// @note Call this function whenever you want the screen to stay/come up
    void light_displayRefresh( void );

    /// @brief Display the current error message of the light
    /// @param msg error message to be displayed
    void light_displayError( char *msg );

    /// @brief Initialise the time keeper and ensure it's running normally
    /// @return false in case of failure to communicate with the time keeper
    bool light_timeInit( void );

    /// @brief Reads and gets the current time and date from the time keeper
    /// @return false if the current time can not be determined, this can be due to failure to communicate with the time keeper
    bool light_timeGet( void );

    /// @brief updates the time keeper with the current date and time
    /// @param dt a string containing both date and time
    /// @note The param should be formated as this "Dec 03 2023 16:35:56"
    bool light_timeSet(String dts);

    /// @brief Get the current day of the week from the time manager 
    /// @return current hour betwwen 0 and 6
    uint8_t light_timeGetDay( void );
    
    /// @brief Get the current hour from the time manager 
    /// @return current hour betwwen 0 and 23
    uint8_t light_timeGetHour( void );
    
    /// @brief Get the current mins that have passe on a given hour from the time manager 
    /// @return current mins betwwen 0 and 59
    uint8_t light_timeGetMin( void );

    /// @brief Reads the light configured states and initilalises the light parameters apppropriately
    void light_serverInit( void );

    /// @brief This function will turn on the server 
    /// @note This function will always start the online service by calling the light_onlineInit function
    /// @retval True if the server starts successfully
    bool light_serverStart( void );

    /// @brief This will turn off the server 
    /// @retval false if the server has been stopped
    bool light_serverStop( void );

    /// @brief This function reads the alarms from memory and populates them in the given array
    /// @param alarms an array where the alarms will be stored after retrieval from memory/storage
    /// @param day The first three character of the day of which the alarms should be retrieved
    /// @retval true if the alarms were populated successfully
    bool light_serveAlarms( LIGHT_ALARM alarms[], String day );

    /// @brief This function saves the cureent light configurations to memory
    /// @note This should be called after the intended configurations have been set.
    /// @retval true if the saving was successfull
    bool light_saveConfigurations( void );

    /// @brief This function saves the given alarms to memory
    /// @param this_day The first three characters of the day to which the alarms should be saved
    /// @param alarm_string A comma separated string containing the alarms to be saved
    void light_updateDayAlarms(String this_day, String alarm_string);

    /// @brief Rstore back the default values/configurations 
    void light_ResetDefaults( void );

    /// @brief get any inputs coming in from the server side
    /// @note this function will also check for commands coming in from online
    /// @return specific command processed by the server
    LIGHT_COMMAND light_serverGetUpdates( void );

    /// @brief Initialise the online service
    /// @note In most cases this will be MQTT or CAOP
    void light_onlineInit( void );

    /// @brief Send a given message to the online platform
    /// @param res_msg A string containing the message to be sent online.
    void light_sendOnline(String res_msg);

    /// @brief This function takes care of all the online business.
    /// @return Any valid command coming through from online
    LIGHT_COMMAND light_getOnlineRequest( void );

    void tryThis( void );

#endif

/* ------------------------------- END OF FILE ------------------------------ */
