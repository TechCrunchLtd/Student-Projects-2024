
#include "box_config.h"
#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>

typedef enum BFP_STATES
{
    BFP_STATE_GETIMAGE,
    BFP_STATE_CONVERTIMAGE,
    BFP_STATE_SEARCHFINGER,
    BFP_STATE_COMPLETE,
    BFP_STATE_ERROR
}
BFP_STATES;


SoftwareSerial bpfSerial(5, 6);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&bpfSerial);
BFP_STATES bfp_current_state = BFP_STATE_GETIMAGE;
uint16_t bfp_user_id;

DRV_STATUS bfp_InitSensor( void )
{
    // Serial.println("\n\nAdafruit finger detect test");
    finger.begin(57600);
    delay(5);
    if (!finger.verifyPassword())
    {
        bfp_current_state = BFP_STATE_ERROR;
        return DRV_STATUS_ERROR;
    }

    Serial.println(F("Finger"));
    finger.getParameters();
    Serial.print(F("Status: 0x")); Serial.println(finger.status_reg, HEX);
    Serial.print(F("Sys ID: 0x")); Serial.println(finger.system_id, HEX);
    // Serial.print(F("Capacity: ")); Serial.println(finger.capacity);
    // Serial.print(F("Security level: ")); Serial.println(finger.security_level);
    // Serial.print(F("Device address: ")); Serial.println(finger.device_addr, HEX);
    // Serial.print(F("Packet len: ")); Serial.println(finger.packet_len);
    // Serial.print(F("Baud rate: ")); Serial.println(finger.baud_rate);

    finger.getTemplateCount();
    if (finger.templateCount == 0)
    {
        Serial.print("No fingerprints");
        bfp_current_state = BFP_STATE_ERROR;
        return DRV_STATUS_WRONG;
    }

    return DRV_STATUS_CORRECT;
}


DRV_STATUS bfp_ReadSensor( void )
{
    uint8_t rw_status = 0;
    bpfSerial.listen();

    switch(bfp_current_state)
    {
        case BFP_STATE_GETIMAGE:
        {
            bfp_user_id = 0;
            rw_status = finger.getImage();
            switch(rw_status)
            {
                case FINGERPRINT_OK: //Imgae taken
                {
                    bfp_current_state = BFP_STATE_CONVERTIMAGE;
                    return DRV_STATUS_WAITING;
                }

                case FINGERPRINT_NOFINGER:
                {
                    return DRV_STATUS_NONE;
                }

                case FINGERPRINT_IMAGEFAIL:
                case FINGERPRINT_PACKETRECIEVEERR:
                {
                    return DRV_STATUS_FAILED;
                }

                default:
                {
                    bfp_current_state = BFP_STATE_ERROR;
                    return DRV_STATUS_ERROR;
                }
            }
            break;
        }

        case BFP_STATE_CONVERTIMAGE:
        {
            rw_status = finger.image2Tz();
            switch(rw_status)
            {
                case FINGERPRINT_OK:
                {
                    bfp_current_state = BFP_STATE_SEARCHFINGER;
                    return DRV_STATUS_WAITING;
                }

                case FINGERPRINT_IMAGEMESS:
                case FINGERPRINT_PACKETRECIEVEERR:
                case FINGERPRINT_FEATUREFAIL:
                case FINGERPRINT_INVALIDIMAGE:
                {
                    bfp_current_state = BFP_STATE_GETIMAGE;
                    return DRV_STATUS_FAILED;
                }

                default:
                {
                    bfp_current_state = BFP_STATE_ERROR;
                    return DRV_STATUS_ERROR;
                }
            }
            break;
        }

        case BFP_STATE_SEARCHFINGER:
        {
            rw_status = finger.fingerSearch();
            // rw_status = finger.fingerFastSearch();
            switch(rw_status)
            {
                case FINGERPRINT_OK:
                {
                    bfp_current_state = BFP_STATE_COMPLETE;
                    bfp_user_id = finger.fingerID;
                    return DRV_STATUS_CORRECT;
                }
                case FINGERPRINT_NOTFOUND:
                {
                    bfp_current_state = BFP_STATE_COMPLETE;
                    return DRV_STATUS_WRONG;
                }

                default:
                {
                    bfp_current_state = BFP_STATE_ERROR;
                    return DRV_STATUS_ERROR;
                }
            }
            break;
        }

        case BFP_STATE_COMPLETE:
        {
            bfp_current_state = BFP_STATE_GETIMAGE;
            return DRV_STATUS_NONE;
            break;
        }

    }

    return DRV_STATUS_NONE;
}

BOX_USERS bfp_GetFingerPrintUser( void )
{
    Serial.print("ID:"); Serial.println(bfp_user_id);
    if(bfp_user_id == 0)
    {
        return UNKNOWN;
    }
    else if(bfp_user_id == 0x05)
    {
        return PERSON_1;
    }
    else if(bfp_user_id == 0x06)
    {
        return PERSON_2;
    }
    else
    {
        return PERSON_3;
    }
    
    return UNKNOWN;
}

