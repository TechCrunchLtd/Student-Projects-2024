

#define LED               LED_BUILTIN
#define ON                true
#define OFF               false

#include <ESP8266WiFi.h>
#include <PubSubClient.h>         // for MQTT

#define SEND_INTERVAL_SECS 5

/* ----------------------------- WIFI CONNECTION ---------------------------- */
  const char* ssid = "boffins_mobile";
  const char* password = "Boffins@13";
  WiFiClient wifiClient;
  
  /// @brief connect to wifi using the given credentials
  void WIFI_init() 
  {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
    
    }
  }

/* ----------------------------- WIFI CONNECTION ---------------------------- */

/* ---------------------------- MQTT COMMUNICATION --------------------------- */
  PubSubClient mqtt(wifiClient);  //connect with wifi

  const char* broker = "broker.hivemq.com";                    // Public IP address or domain name
  const char* mqttUsername = "";  // MQTT username
  const char* mqttPassword = "";  // MQTT password
  const char* mqtt_sub_topic = "ToLocalNDU2406";
  const char* mqtt_pub_topic = "FromLocalNDU2406";
  
  /// @brief connect to MQTT broker
  void mqttConnect() 
  {
    uint8_t mqtt_tries = 0;
    while(!mqtt.connected()){
      // Connect to MQTT Broker without username and password
      if(mqtt.connect("NDU2406")){
        mqtt.subscribe(mqtt_sub_topic);
        return;
      }
      else{
        if(++mqtt_tries > 4) ESP.restart();
        toggleLED(10);
      }
    }
  }

  /// @brief handle incoming messages from broker
  void mqttCallback(char* topic, byte* message, unsigned int len) 
  {
    String messageTemp;
    
    for (int i = 0; i < len; i++) {
      messageTemp += (char)message[i];
    }

  }

  /// @brief MQTT Broker setup and connect to broker
  void MQTT_init()
  {
    mqtt.setServer(broker, 1883);
    mqtt.setCallback(mqttCallback);
    mqttConnect();
  }
  
  /// @brief publish message to the set mqtt_pub_topic
  void mqttPublish(String msg_str)
  {
    char pub_msg[6];
      sprintf(pub_msg, "%s", msg_str);
    
    mqtt.publish(mqtt_pub_topic, pub_msg);
  }
/* ---------------------------- END OF MQTT COMMUNICATION --------------------------- */


/// @brief switch LED on or off
/// @param new_state to switch the line 
void switchLED( bool new_state)
{
  if(new_state == digitalRead(LED)){
    digitalWrite(LED, !new_state);
  }
}

void toggleLED( int count)
{
    for(int i=0; i<count; i++){
        switchLED(ON); delay(250);
        switchLED(OFF);  delay(250);
    }
}

void setup()
{
    Serial.begin(9600);
    pinMode(LED, OUTPUT);
    toggleLED(4);
  
    WIFI_init();
    toggleLED(4);

    MQTT_init();
    switchLED(OFF);
}

unsigned long sendMillis;

void loop()
{
    serialEvent();
    if (!mqtt.connected()) {
        mqttConnect();
    }

    serialEvent();
    if(millis() - sendMillis > SEND_INTERVAL_SECS*3*1000)
    {
        mqttPublish("No data");
        sendMillis = millis();
        Serial.println(millis());
    }
    serialEvent();
    mqtt.loop();
}

/// @brief read incoming bytes from atmega328p
void serialEvent()
{
  if(Serial.available())
  {
    String msg = "";
    char c = Serial.read();
    if( c == '*'){
      msg = Serial.readStringUntil('#');
          msg.trim();
    }
    while(Serial.available()) c = Serial.read();

      if(msg.length() > 2){
          // switchLED(ON);
          mqttPublish(msg);
          sendMillis = millis();
    }
  } 
}

/* ----------------------------------- EOF ---------------------------------- */
