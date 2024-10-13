
#include "box_config.h"
#include <SoftwareSerial.h>


SoftwareSerial SerialGSM(11, 12);  // RX, TX pins for SoftwareSerial
String phoneNumbers[] = {"+256782028592", "+256779921629", "+256787670957", "+256764064549"};
char gsm_message[4][12] = {"BAD", "1234", "SAFE OPENED", "SAFE CLOSED"};
bool message_received;
uint8_t send_message;
uint8_t message_count;

GSM_RESP readSerial(int wait_ms)
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
            return GSM_SMS;
        }

        return GSM_ACK;
    }

    return GSM_NACK;
}

DRV_STATUS gsm_InitModule( void )
{
    bool gsm_ready = false;
    delay(5000);
    SerialGSM.begin(4800); // use for init module. Use it if you dont have any valid reason.
    delay(5000);

    SerialGSM.println("AT"); //Once the handshake test is successful, it will back to OK
    if(readSerial(500) == GSM_ACK)
    {
        gsm_ready = true;
    }
    delay(100);
    SerialGSM.println("AT+CMGF=1"); // Configuring TEXT mode
    readSerial(500);
    SerialGSM.println("AT+CNMI=2,1,0,0,0");
    readSerial(500);

    message_received = false;
    send_message = 0;

    if(gsm_ready)
    {
        return DRV_STATUS_CORRECT;
    }

    return DRV_STATUS_ERROR;
}

DRV_STATUS gsm_StateMachineRun( void )
{
    SerialGSM.listen(); //listen to incoming data
    GSM_RESP gsm_resp = GSM_NACK;
    if(message_received)
    {
        SerialGSM.println("AT+CMGR=1"); // READ FIRST MESSAGE
        gsm_resp = readSerial(500);
        SerialGSM.println("AT+CMGD=1"); // DELET LAST RECEIVED MESSAGE
        readSerial(500);
        message_received = false;

        if(gsm_resp == GSM_SMS)
        {
            return DRV_STATUS_CORRECT;
        }
        else
        {
            return DRV_STATUS_WRONG;
        }
    }

    if(send_message != 0)
    {
        // Serial.print("SEND: "); Serial.println(send_message);
        SerialGSM.println("AT+CMGF=1");
        readSerial(500);
        // SerialGSM.println("AT+CMGS=\"+256787934219\"");
        if(message_count == 4)
        {
            SerialGSM.println("AT+CMGS=\"+256787670957\"");
        }
        else if(message_count == 3)
        {
            SerialGSM.println("AT+CMGS=\"+256779921629\"");
        }
        else if(message_count == 2)
        {
            SerialGSM.println("AT+CMGS=\"+256782028592\"");
        }
        else if(message_count == 1)
        {
            SerialGSM.println("AT+CMGS=\"+256787934219\"");
        }
        
        // "", "", "+256787670957", ""
        readSerial(500);
        SerialGSM.print(gsm_message[send_message]);
        readSerial(500);
        SerialGSM.write(26);
        readSerial(3000);
        message_count--;
        if(message_count < 1)
        {
            send_message = 0;
            return DRV_STATUS_CORRECT;

        }

        return DRV_STATUS_WAITING;
 
    }

    readSerial(1);
    return DRV_STATUS_NONE;
}

DRV_STATUS gsm_SetMessage( uint8_t msgid)
{
    if(msgid < 1 || msgid > 3)
    {
        return DRV_STATUS_FAILED;
    }

    send_message = msgid;
    message_count = 4;
    return DRV_STATUS_CORRECT;
}

/* ----------------------------------- EOF ---------------------------------- */