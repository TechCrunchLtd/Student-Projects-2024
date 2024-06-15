/**
 * @file patient_config.h
 * @defgroup home_files
*/

#ifndef DEVICE_CONFIG_H
    #define DEVICE_CONFIG_H

    /** @brief - When debugging using the serial monitor and nodemcu */
    // #define DEVICE_DEBUG


    /** @brief - Defines the maximum allowed string length for any given variable in the program */
    #define DEVICE_MAX_STR_LEN    25

    typedef enum DEVICE_COMMAND
    {
        CMD_NONE,

        /** @brief - Command to switch off/on the power line */
        CMD_SWITCH_LINE,

        /** @brief - Command to switch the local server on/off 
         * @note - The local server is needed to switch current operator numbers
        */
        CMD_SWITCH_SERVER,

        /** @brief - Command to restore factory settings */
        CMD_DEVICE_RESET
    }
    DEVICE_COMMAND;

    /** @brief - Defines all the different types of errors to expect*/
    typedef enum DEVICE_ERROR
    {
        /** @brief - No errors so far */
        NO_ERROR,

        /** @brief - Errors to do with the power line being monitored, for example, when a short circuit has occured */
        LINE_ERROR,

        /** @brief - Errors to do with the GSM communication, for example, GSM has no network */
        GSM_ERROR,

        /**@brief - Errors to do with file handling, for example, if a file fails to open or can't be written */
        FILE_ERROR,

        /** @brief - Communication errors, for example, failure to connect to ESP-now peer */
        COMM_ERROR
    }
    DEVICE_ERROR;

    /** @brief - Defines all the parameters that the light works with be it volatile or non-volatile parameters*/
    typedef struct DEVICE_PARAMS
    {
        char server_IP[DEVICE_MAX_STR_LEN];
        char server_name[DEVICE_MAX_STR_LEN];
        char server_password[DEVICE_MAX_STR_LEN];
        char error_message[DEVICE_MAX_STR_LEN];
        char user_number[DEVICE_MAX_STR_LEN];

        /** @brief - holds the current error yet to be cleared */
        DEVICE_ERROR device_error;

        /** @brief - holds the current state of the server */
        bool server_running;


        /** @brief - holds the current value of the last read voltage */
        float line_voltage;

        /** @brief - holds the current value of the last read current */
        float line_current;

        /** @brief - holds the current line error code */
        uint8_t line_fault;

        /** @brief - holds the current count of errors logged */
        uint8_t log_count;

        /** @brief - holds the current value of the last read heart rate */
        bool line_status;
    }
    DEVICE_PARAMS;

    extern DEVICE_PARAMS device_params;
     

    /* ---------------------- Lets prototype all functions ---------------------- */
    
    /// @brief set the current device error
    /// @return true if error setting was successful
    bool device_setError( DEVICE_ERROR error);

    /// @brief This function should be called to in case of any error
    /// @param new_error the exact type of error that has occured
    /// @note The next state will be determined by the type  of error submitted
    void device_errorHandler( DEVICE_ERROR new_error);

    /// @brief setup the peripherals to be used as primary inpput
    // bool device_inputSetup( void );

    /// @brief read status commands from the primary input device
    /// @return the specific commandas processed by the input device
    // DEVICE_COMMAND device_inputRead(void);

    /// @brief Checks if the display is available and boots it up
    /// @return false if the display can not be found
    bool device_displayInit( void );

    /// @brief update the display with the given information
    void device_displayUpdate(bool config);

    /// @brief Display a given error message
    /// @param error is a pointer the error message string which is to be displayed
    /// @attention This function does not return, the code will stay in this function until an external reset occurs
    void device_displayError( char *error );

    /// @brief Reads the device configured states and initilalises the device parameters from memory apppropriately
    /// @note This function will always start the local service depending on the last configured state
    bool device_serverInit( void );

    /// @brief This function will turn on the server 
    /// @retval True if the server starts successfully
    bool device_serverStart( void );

    /// @brief This will turn off the server 
    /// @retval false if the server has been stopped
    bool device_serverStop( void );

    /// @brief get any inputs coming in from the server side
    /// @return false if there is no connected client on the server
    bool device_serverUpdate( void );

    /// @brief log every fault that occurs
    /// @param info the fault information to be logged
    /// @return false if logging failed
    bool device_serverLogFault(char *info);

    /// @brief This function saves the current device configurations to memory
    /// @note This should be called after the intended configurations have been set.
    /// @retval true if the saving was successfull
    bool device_saveConfigurations( void );

    /// @brief Rstore back the default values/configurations 
    void device_ResetDefaults( void );

    /// @brief Initialise the gsm messaging service
    /// @note This function is blocking unitl the gsm service is ready
    bool device_gsmInit( void );

    /// @brief Send a given message to the current operator
    /// @param res_msg A string containing the message to be sent online.
    bool device_gsmSendSMS(char *res_msg);

    /// @brief Initialize the espnow communication service
    /// @return true if communication has been established
    bool device_espnowInit( void );

    /// @brief Start the espnow communication
    /// @return true if the communication is active
    bool device_espnowStart( void );

    /// @brief Sop the espnow communication
    /// @return false if the communication is inactive
    bool device_espnowStop( void );

    /// @brief This function takes care of all the espnow business.
    /// @return true if communication is still active
    /// @note This function does not handle local server requests but only espnow communications
    bool device_espnowUpdate( void );

#endif

/* ------------------------------- END OF FILE ------------------------------ */
