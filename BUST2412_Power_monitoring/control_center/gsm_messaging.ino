#include "device_config.h"
#define SerialGSM   Serial


bool readSerial(int wait_ms, char* token)
{
    delay(wait_ms);
    if(SerialGSM.available())
    {
        String res = "";
        res = SerialGSM.readString();
        res.toUpperCase();
        if(res.indexOf(token) != -1)
        {
            char resp[80];
            res.toCharArray(resp, 80);
            device_displayError(resp);
            return true;
        }
    }

    return false;
}

bool device_gsmInit( void )
{
    bool gsm_ready = false;
    device_displayError("waiting for GSM");
    delay(5000);
    SerialGSM.begin(9600); // use for init module. Use it if you dont have any valid reason.
    delay(5000);

    SerialGSM.println("AT"); //Once the handshake test is successful, it will back to OK
    gsm_ready = readSerial(500, "OK");
    delay(100);
    // SerialGSM.println("AT+CMGF=1"); // Configuring TEXT mode
    // gsm_ready = readSerial(500, "OK");
    // SerialGSM.println("AT+CNMI=2,1,0,0,0");
    gsm_ready = readSerial(500, "OK");
    device_displayError("SMS mode set");
    delay(1000);

    return gsm_ready;
}

bool device_gsmSendSMS(char *res_msg)
{
    device_displayError("Sending sms");
    delay(1000);
    // device_displayError(res_msg);
    
    // static uint8_t gsm_line_fault = 0;

    // if(device_params.line_fault != gsm_line_fault)
    // {
        device_displayError("Sending sms...");
        SerialGSM.println("AT+CMGF=1");
        readSerial(500, "OK");
        // SerialGSM.println("AT+CMGS=\"+256782912385\"");
        SerialGSM.print("AT+CMGS=\"");
        SerialGSM.print(device_params.user_number);
        SerialGSM.println("\"");
        readSerial(500, "OK");
        SerialGSM.print(res_msg);
        readSerial(500, "OK");
        SerialGSM.write(26);
        readSerial(500, "OK");
        
        device_displayError("Sending sms... done");
        delay(1000);
        // gsm_line_fault = device_params.line_fault;
    // }

    // return readSerial(1, "OK");
    return true;

}

/* ----------------------------------- EOF ---------------------------------- */
