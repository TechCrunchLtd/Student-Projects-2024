
#include <ESP8266WiFi.h>
#include <espnow.h>

#define DEVICE_DEBUG
#define LED_PIN     2

/// @brief MAC address of the receiver device --> 34:94:54:95:85:16
uint8_t recieverAddress[] = {0x34, 0x94, 0x54, 0x95, 0x85, 0x16};

char currentData[100] = "";
unsigned long dataSentMillis;
unsigned long dataRecvMillis;

uint8_t blinkRate = 0;


typedef struct LINE_PARAMS
{
    uint16_t voltage;
    uint16_t current;
    uint8_t fault;
    uint8_t line_status;
} LINE_PARAMS;

LINE_PARAMS line_params;

void getMACAddress()
{
    WiFi.mode(WIFI_STA);

    while(1)
    {
        Serial.println(WiFi.macAddress());
        delay(2000);
    }

}

/// @brief trun on the led and blink it
/// @param error led stays on if error is set to true
/// @param secs the interval of blinking
void blinkLED(bool error, uint8_t secs)
{
    static bool errorOn = false;

    if(error)
    {
        if(errorOn == false)
        {
            errorOn = true;
            blinkRate = secs/2;
        }
        return;
    }

    errorOn = false;
    blinkRate = 0;
    
}

/// @brief check on the led status and keep it going
void blinkStatus()
{
    static unsigned long blink_time = 0;
    unsigned long blink_rate = blinkRate*1000;

    if(blinkRate == 0)
    {
        blink_rate = 300;
    }

    if(millis() - blink_time >= blink_rate)
    {
        blink_time = millis();
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    }
}

void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus)
{
    if(sendStatus == 0)
    {  
#ifdef DEVICE_DEBUG
    Serial.println("Data sent!");
#endif
        blinkLED(false, 0); //blink led for success
    }
}

/// @brief Callme back for incoming data
/// @param mac address of the sender
/// @param incomingData pointer for received data
/// @param len total bytes of data received
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len)
{
    char recvd[20];
    memcpy(recvd, incomingData, len);  
#ifdef DEVICE_DEBUG
    Serial.print("recv: ");
    Serial.println(recvd);
#endif

    if(strstr(recvd, "OK"))
    {
        blinkLED(false, 0); //blink led for good communication
        dataRecvMillis = millis();
    }
}

void setup()
{
    Serial.begin(9600);
    // getMACAddress(); //run on first time to get device's address

    pinMode(LED_PIN, OUTPUT);

    WiFi.mode(WIFI_STA); // Set device as a Wi-Fi Station
    WiFi.disconnect();

    // Init ESP-NOW
    if (esp_now_init() != 0) 
    {
        blinkLED(true, 10);
        return;
    }

    esp_now_set_self_role(ESP_NOW_ROLE_COMBO); // Set ESP-NOW Role

    // Once ESPNow is successfully Init, we will register for Send CB to
    // get the status of Trasnmitted packet
    esp_now_register_send_cb(OnDataSent);

    // Register peer
    esp_now_add_peer(recieverAddress, ESP_NOW_ROLE_COMBO, 1, NULL, 0);

    // Register for a callback function that will be called when data is received
    esp_now_register_recv_cb(OnDataRecv);

    dataSentMillis = millis();
    dataRecvMillis = millis();
}

void loop()
{
    //wait for message from line monitor
    if(Serial.available())
    {
        String msg = "";
        char c = Serial.read();
        if(c == '*')
        {
            msg = Serial.readStringUntil('#');
            msg.trim();
        }
        while(Serial.available()) c = Serial.read();
  
#ifdef DEVICE_DEBUG
    Serial.print("Recv: ");
    Serial.println(msg);
#endif
        if(msg.length() > 3)
        {  
            int index = msg.indexOf(',');
            String data = msg.substring(0, index);
            
#ifdef DEVICE_DEBUG
    Serial.print("volt: ");
    Serial.println(data);
#endif
            line_params.voltage = data.toInt();
            msg = msg.substring(index+1);

            index = msg.indexOf(',');
            data = msg.substring(0, index);
            
#ifdef DEVICE_DEBUG
    Serial.print("curr: ");
    Serial.println(data);
#endif
            line_params.current = data.toInt();
            msg = msg.substring(index+1);

            index = msg.indexOf(',');
            data = msg.substring(0, index);
            
#ifdef DEVICE_DEBUG
    Serial.print("faut: ");
    Serial.println(data);
#endif
            line_params.fault = data.toInt();
            msg = msg.substring(index+1);
            
#ifdef DEVICE_DEBUG
    Serial.print("line: ");
    Serial.println(msg);
#endif

            line_params.line_status = msg.toInt();

#ifdef DEVICE_DEBUG
    Serial.println("Sending data");
    Serial.println(line_params.voltage);
    Serial.println(line_params.current);
    Serial.println(line_params.fault);
    Serial.println(line_params.line_status);
#endif
            // Send message via ESP-NOW
            esp_now_send(recieverAddress, (uint8_t *) &line_params, sizeof(line_params));
            dataSentMillis = millis();
        }
    }


    //if timer expires without receiving message from line monitor, set error
    if((millis() - dataSentMillis) > 5000)
    {
        blinkLED(true, 2);
    }

    //if timer expires without receiving message from control ceneter, set error
    if((millis() - dataRecvMillis) > 5000)
    {
        blinkLED(true, 4);
    }

    blinkStatus();
}