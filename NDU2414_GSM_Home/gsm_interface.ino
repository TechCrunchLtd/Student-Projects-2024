
#include "gsm_home.h"
#include <SoftwareSerial.h>
// #define SerialGSM Serial
SoftwareSerial SerialGSM(3, 2); //SIM800L Tx & Rx is connected to Arduino #3 & #2
bool message_received;

HOME_CMD readSerial(int wait_ms)
{
    delay(wait_ms);
    if(SerialGSM.available())
    {
        String res = "";
        res = SerialGSM.readString();
        res.toUpperCase();
        if(res.indexOf("CMTI") != -1)
        {
            message_received = true;
        }
        else if(res.indexOf("1 ON") != -1)
        {
            return LOAD_ON;
        }
        else if(res.indexOf("1 OFF") != -1)
        {
            return LOAD_OFF;
        }
        else if(res.indexOf("RING") != -1)
        {
            return LOAD_TOGGLE;
        }
    }

    return NO_CMD;
}

void gsm_init(void)
{
    delay(5000);
    SerialGSM.begin(9600); // use for init module. Use it if you dont have any valid reason.
    delay(5000);

    SerialGSM.println("AT"); //Once the handshake test is successful, it will back to OK
    readSerial(500);
    delay(100);
    SerialGSM.println("AT+CMGF=1"); // Configuring TEXT mode
    readSerial(500);
    SerialGSM.println("AT+CNMI=2,1,0,0,0");
    readSerial(500);

    message_received = false;
}

HOME_CMD gsm_run(void)
{
    HOME_CMD sms_cmd = NO_CMD;

    if(message_received)
    {
        SerialGSM.println("AT+CMGR=1"); // READ FIRST MESSAGE
        sms_cmd = readSerial(500);
        SerialGSM.println("AT+CMGD=1"); // DELET LAST RECEIVED MESSAGE
        readSerial(500);
        message_received = false;

        return sms_cmd;
    }

    if(sms_cmd == NO_CMD)
        sms_cmd = readSerial(1);

    if(sms_cmd == LOAD_TOGGLE)
    {
        SerialGSM.println("ATH");//hung up call
        readSerial(500);
    }
    return sms_cmd;
}