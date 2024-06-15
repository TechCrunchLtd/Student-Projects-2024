#include "home_config.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h> // for MQTT

WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);                // connect with wifi
const char *mqttBroker = "broker.hivemq.com"; // Public IP address or domain name
const char *clientId = "TechCruncher0000"; //unique id for each device
char mqtt_pub_topic[LIGHT_MAX_STR_LEN] = "Pub_256784356760";
char mqtt_sub_topic[LIGHT_MAX_STR_LEN] = "Sub_256784356760";
char mqtt_pub_msg[LIGHT_MAX_STR_LEN*3] = "";

const unsigned char lightOffed [] PROGMEM =  "Light already off";
const unsigned char lightOff   [] PROGMEM =  "Light turned off";
const unsigned char lightOned  [] PROGMEM =  "Light already on";
const unsigned char lightOn    [] PROGMEM =  "Light turned on";
const unsigned char queryOff   [] PROGMEM =  "Light off";
const unsigned char queryoff   [] PROGMEM =  "light off";
const unsigned char queryOn    [] PROGMEM =  "Light on";
const unsigned char queryon    [] PROGMEM =  "light on";
const unsigned char queryAdd   [] PROGMEM =  "Address";
const unsigned char queryadd   [] PROGMEM =  "address";
const unsigned char queryFail  [] PROGMEM =  "Update couldn't be applied!";
const unsigned char invalid    [] PROGMEM =  "Your query was invalid!";


unsigned long previousMillis = 0;
bool previousLightState = false;
bool onlineTrigered = false;
bool sentAddress = false;

LIGHT_COMMAND onlineCommand;

/// @brief publish message to the set mqtt_pub_topic
void light_sendOnline(char *send_msg)
{
    // char pub_msg[62];
    // sprintf(pub_msg, "%s", res_msg);
    // res_msg.toCharArray(pub_msg, 62);
    mqtt.publish(mqtt_pub_topic, send_msg);
}

/// @brief Decode the received request and take appropriate action
/// @param query
/// @return a string responce about the new state
void HandleRequest(char *req)
{
    String query = String(req);
    query.toLowerCase();
    memset(mqtt_pub_msg, '\0', LIGHT_MAX_STR_LEN*3);

    if (query == "light on")
    {
        if (light_params.light_activated)
        {
            sprintf(mqtt_pub_msg, "%s", lightOned);
            return;
        }

        onlineCommand = SWITCH_LIGHT;
        onlineTrigered = true;

        sprintf(mqtt_pub_msg, "Done: %s", lightOn);
        return;
    }

    else if (query == "light off")
    {
        if (!light_params.light_activated)
        {
            sprintf(mqtt_pub_msg, "%s", lightOffed);
            return;
        }
        onlineCommand = SWITCH_LIGHT;
        onlineTrigered = true;

        sprintf(mqtt_pub_msg, "Done: %s", lightOff);
        return;
    }

    else if (query == "light status")
    {
        if(light_params.light_activated)
        {
            sprintf(mqtt_pub_msg, "%s", lightOn);
        }
        else
        {
            sprintf(mqtt_pub_msg, "%s", lightOff);
        }
        return;
    }

    else if (query == "address")
    {
        sprintf(mqtt_pub_msg, "IP %s", light_params.server_address);
        return;
    }

    else if (query.indexOf("update:") != -1)
    {
        String str_val = query.substring(query.indexOf(":") + 1);
        int new_value = str_val.toInt();
        
        if (new_value > 0)
        {
            light_params.update_interval = new_value;
            light_params.light_alarm_on = true;
            light_saveConfigurations();

            sprintf(mqtt_pub_msg, "Done: %u", light_params.update_interval);
            return;
        }

        sprintf(mqtt_pub_msg, "%s", queryFail);
        return;
    }

    else
    {
        sprintf(mqtt_pub_msg, "%s", invalid);
        return;
    }
}

/// @brief handle incoming messages from broker
void mqttCallback(char *topic, byte *message, unsigned int len)
{
    // String message_temp;
    memset(mqtt_pub_msg, '\0', LIGHT_MAX_STR_LEN*3);
        

    for (int i = 0; i < len; i++)
    {
        sprintf(mqtt_pub_msg, "%s%c", mqtt_pub_msg, (char)message[i]);
        // message_temp += (char)message[i];
    }

// #ifdef USING_NODEMCU
//     Serial.println(message_temp);
//     Serial.flush();
// #endif
    HandleRequest(mqtt_pub_msg);
    light_sendOnline(mqtt_pub_msg);
    // light_sendOnline(mqtt_pub_msg);
}

/// @brief connect to MQTT broker
void mqttConnect()
{
    if (!mqtt.connected())
    {
        // Connect to MQTT Broker without username and password
        if (mqtt.connect(clientId))
        {
            mqtt.subscribe(mqtt_sub_topic);
#ifdef USING_NODEMCU
            Serial.println("Mqtt connected and subscribed");
            Serial.println(mqtt_pub_topic);
            Serial.println(mqtt_sub_topic);
#endif
            if(!sentAddress){
                memset(mqtt_pub_msg, '\0', LIGHT_MAX_STR_LEN*3);
                sprintf(mqtt_pub_msg, "Address %s", light_params.server_address);
                
                light_sendOnline(mqtt_pub_msg);
                sentAddress = true;
            }
        }
    }
}

void light_onlineInit( void )
{
    sprintf(mqtt_sub_topic, "Sub_%s", light_params.whatsapp_number);
    sprintf(mqtt_pub_topic, "Pub_%s", light_params.whatsapp_number);
    if (light_params.server_running)
    {
        mqtt.setServer(mqttBroker, 1883);
        mqtt.setCallback(mqttCallback);
        mqttConnect();
    }

    previousLightState = light_params.light_activated;
    previousMillis = millis();
}

LIGHT_COMMAND light_getOnlineRequest( void )
{
    if (!mqtt.connected())
    {
#ifdef USING_NODEMCU
        Serial.println("Mqtt connection lost!");
#endif
        mqttConnect();
    }

    if ((light_params.light_alarm_on != false) && ((millis() - previousMillis) >= (light_params.update_interval * 60 * 1000)))
    {
        memset(mqtt_pub_msg, '\0', LIGHT_MAX_STR_LEN*3);
        // sprintf(mqtt_pub_msg, "Light is currently %s", (light_params.light_activated ? "ON" : "OFF"));
        if(light_params.light_activated == true) {
            sprintf(mqtt_pub_msg, "%s", lightOn);
        }
        else{
            sprintf(mqtt_pub_msg, "%s", lightOff);
        }
        
        light_sendOnline(mqtt_pub_msg);

        previousMillis = millis();
    }

    if (light_params.light_activated != previousLightState)
    {
        previousLightState = light_params.light_activated;

        if(!onlineTrigered)
        {
            memset(mqtt_pub_msg, '\0', LIGHT_MAX_STR_LEN*3);
            // sprintf(mqtt_pub_msg, "Light turned %s locally", (previousLightState ? "ON" : "OFF"));
            if(previousLightState == true) {
                sprintf(mqtt_pub_msg, "%s locally", lightOn);
            }
            else{
                sprintf(mqtt_pub_msg, "%s locally", lightOff);
            }
            
            light_sendOnline(mqtt_pub_msg);
        }

        onlineTrigered = false;
    }

    mqtt.loop();

    LIGHT_COMMAND new_command = onlineCommand;
    onlineCommand = NO_COMMAND;

    return new_command;
}

/* ----------------------------------- EOF ---------------------------------- */
