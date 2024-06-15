#include "patient_config.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h> // for MQTT

WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);                // connect with wifi
const char *mqttBroker = "broker.hivemq.com"; // Public IP address or domain name
const char mqtt_pub_topic[DEVICE_MAX_STR_LEN] = "patients_must2401";
const char mqtt_sub_topic[DEVICE_MAX_STR_LEN] = "Sub_256782912385";

unsigned long previousMillis = 0;

/// @brief publish message to the set mqtt_pub_topic
bool device_onlineSend(String res_msg)
{
    char pub_msg[62];
    // sprintf(pub_msg, "%s", res_msg);
    res_msg.toCharArray(pub_msg, 62);
    
     return mqtt.publish(mqtt_pub_topic, pub_msg);
}

/// @brief Decode the received request and take appropriate action
/// @param query
/// @return a string responce about the new state
String HandleRequest(String query)
{
    query.toLowerCase();
    
    if (query.indexOf("interval:") != -1)
    {
        // get substring with
        String str_val = query.substring(query.indexOf(":") + 1);
        int new_value = str_val.toInt();
        Serial.print("str: ");
        Serial.print(str_val);
        Serial.print("\t\tInt: ");
        Serial.println(new_value);

        if (new_value > 0)
        {
            device_params.update_interval = new_value;
            device_params.updates_enabled = true;
            device_saveConfigurations();

            String ret_msg = "Interval is now ";
            ret_msg += String(device_params.update_interval) + " mins";
            return ret_msg;
        }

        return "New interval couldn't be set";
    }

    else
        return "Not interval setting";
}

/// @brief handle incoming messages from broker
void mqttCallback(char *topic, byte *message, unsigned int len)
{
    String message_temp;

    for (int i = 0; i < len; i++)
    {
        message_temp += (char)message[i];
    }

#ifdef DEVICE_DEBUG
    Serial.println(message_temp);
    Serial.flush();
#endif

    HandleRequest(message_temp);
}

/// @brief connect to MQTT broker
bool mqttConnect()
{
    if (!mqtt.connected())
    {
        // Connect to MQTT Broker without username and password
        if (mqtt.connect("TechCrunchLimited03"))
        {
            mqtt.subscribe(mqtt_sub_topic);
#ifdef DEVICE_DEBUG
            Serial.println("Mqtt connected and subscribed");
            Serial.println(mqtt_pub_topic);
            Serial.println(mqtt_sub_topic);

#endif
            device_params.device_error = NO_ERROR;
            return true;
        }

        return false;
    }

    return true;
}

/// @brief MQTT Broker setup and connect to broker
bool MQTT_init()
{
    mqtt.setServer(mqttBroker, 1883);
    mqtt.setCallback(mqttCallback);
    return mqttConnect();
}

/* ---------------------------- END OF MQTT COMMUNICATION --------------------------- */

bool device_onlineInit( void )
{
    bool online = false;
    if (device_params.server_running)
    {
        online = MQTT_init();
    }

    previousMillis = millis();
    return online;
}

bool device_onlineRequestHandler( void )
{
    if (!mqtt.connected())
    {
#ifdef DEVICE_DEBUG
        Serial.println("Mqtt connection lost!");
#endif
        device_params.device_error = COMM_ERROR;
        sprintf(device_params.error_message, "Connection lost!");
        mqttConnect();
    }
    
    if ((device_params.updates_enabled != false) && ((millis() - previousMillis) >= (device_params.update_interval * 1000)))
    {
#ifdef DEVICE_DEBUG
        Serial.println("Attempting to send online!");
#endif
        String msg = "";
        msg = "{\"dev\":\"" + String(device_params.device_id) + "\",";
        msg += "\"temp\":" + String(device_params.temperature) + ",";
        msg += "\"pulse\":" + String(device_params.heart_rate) + ",";
        msg += "\"spo2\":" + String(device_params.oxygen_level) + "}";

        bool sent = device_onlineSend(msg);
        if(sent == false){
            device_params.device_error = COMM_ERROR;
            sprintf(device_params.error_message, "Sending falied!");
#ifdef DEVICE_DEBUG
        Serial.println("Sending Data online falied!");
#endif
        }

        previousMillis = millis();
    }

    mqtt.loop();
    return true;
}

/* ----------------------------------- EOF ---------------------------------- */
