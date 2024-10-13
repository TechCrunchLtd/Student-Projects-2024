
#ifndef BOX_CONFIG_H
    #define BOX_CONFIG_H

    typedef enum GSM_RESP
    {
        GSM_NACK,
        GSM_ACK,
        GSM_SMS
    }
    GSM_RESP;

    typedef enum DRV_STATUS
    {
        DRV_STATUS_NONE,
        DRV_STATUS_WAITING,
        DRV_STATUS_CORRECT,
        DRV_STATUS_WRONG,
        DRV_STATUS_FAILED,
        DRV_STATUS_ERROR
    }
    DRV_STATUS;

    typedef enum BOX_USERS
    {
        UNKNOWN,
        PERSON_1,
        PERSON_2,
        PERSON_3
    }
    BOX_USERS;

    DRV_STATUS bdp_InitDisplay( void );

    DRV_STATUS bdp_UpdateDisplay( void );

    DRV_STATUS bdp_DirectDisplay( char *msg );

    DRV_STATUS bdp_SetDisplayMsg( const char *msg, uint8_t line_nb);

    DRV_STATUS gsm_InitModule( void );

    DRV_STATUS gsm_StateMachineRun( void );

    DRV_STATUS gsm_SetMessage( uint8_t msgid);

    DRV_STATUS bfp_InitSensor( void );

    DRV_STATUS bfp_ReadSensor( void );

    BOX_USERS bfp_GetFingerPrintUser( void );

#endif