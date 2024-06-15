
#include "home_config.h"
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

#define LIGHT_SERVER_NAME "autolight"
#define LIGHT_USERNAME "admin"
#define LIGHT_PASSWORD "admin"
#define CLIENT_TIMEOUT_MINS 5

ESP8266WebServer server(80);

LIGHT_COMMAND server_command = NO_COMMAND;

bool has_logged_on = false;

unsigned long client_millis;

bool appendFile(String dir, String text)
{
    File file = LittleFS.open(dir, "a");

    if (!file)
    {
        light_errorHandler(FILE_ERROR);
        return false;
    }

    text += "\n";
    file.print(text);
    file.close();
    return true;
}

bool writeFile(String dir, String text)
{
    File file = LittleFS.open(dir, "w");

    if (!file)
    {
        light_errorHandler(FILE_ERROR);
        return false;
    }

    file.print(text);
    delay(1);
    file.close();
    return true;
}

String getContentType(String filename)
{
    if (filename.endsWith(".html"))
        return "text/html";
    else if (filename.endsWith(".css"))
        return "text/css";
    else if (filename.endsWith(".js"))
        return "application/javascript";
    else if (filename.endsWith(".ico"))
        return "image/x-icon";
    else if (filename.endsWith(".png"))
        return "image/png";
    else if (filename.endsWith(".gif"))
        return "image/gif";
    else if (filename.endsWith(".jpg"))
        return "image/jpeg";
    else if (filename.endsWith(".jpeg"))
        return "image/jpeg";
    return "text/plain";
}

bool handleFileRequests(String path)
{
    if (path.endsWith("/"))
    {
        path += "app.html";
    }

    String contentType = getContentType(path);
    path = "/webapp" + path;

    if (LittleFS.exists(path))
    {
        File file = LittleFS.open(path, "r");
        server.streamFile(file, contentType);
        file.close();
        return true;
    }

    return false;
}

void serveUpdates(void)
{
    client_millis = millis();

    String res = "{";
    res += "\"allowed\":" + String(has_logged_on) + ",";
    res += "\"activated\":" + String(light_params.light_activated) + "}";
    // res += "\"alarm\":\"" + String(light_params.next_alarm) + "\",";
    // res += "\"time\":\"" + String(light_params.current_date) + " " + String(light_params.current_time) + "\"}";

#ifdef USING_NODEMCU
    Serial.println(res);
#endif

    server.send(200, "text/plain", res);
}

void toggleLight(void)
{
    if (has_logged_on)
    {
        String res = server.arg("Switch");

#ifdef USING_NODEMCU
        Serial.println(res);
#endif

        if (res == "LIGHT")
        {
            server_command = SWITCH_LIGHT;
            server.send(200, "text/plain", res);
        }
        else
        {
            server.send(206, "text/plain", "Partial Content");
        }
    }
    else
    {
        server.send(401, "text/plain", "Forbiden");
    }
}

void loginClient(void)
{
#ifdef USING_NODEMCU
    Serial.print("Login: ");
    Serial.print(server.arg("Username"));
    Serial.print(" ");
    Serial.println(server.arg("Password"));
#endif

    if (server.arg("Username") == LIGHT_USERNAME && server.arg("Password") == LIGHT_PASSWORD)
    {
        has_logged_on = true;
        server.send(200, "text/plain", "true");
    }
    else
    {
        has_logged_on = false;
        server.send(200, "text/plain", "false");
    }
}

void getAlarms(void)
{
    String t_day = server.arg("Day");
    String path = "/alarms/" + t_day + ".txt";

#ifdef USING_NODEMCU
    Serial.println(path);
#endif

    File file = LittleFS.open(path, "r");
    if (!file)
    {
        server.send(424, "text/plain", "Failed");
    }
    else
    {
        String res = "{\"day\":\"" + t_day + "\",";
        res += "\"alarms\":[\"";

        while (file.available())
        {
            char thischar = file.read();
            if (thischar == '\n')
            {
                res += "\",\"";
            }
            else
            {
                res += thischar;
            }
        }

        file.close();
        res += "\"]}";

#ifdef USING_NODEMCU
        Serial.println(res);
#endif

        server.send(200, "text/plain", res);
    }
}

void light_updateDayAlarms(String this_day, String alarm_string)
{
    String full_path = "/alarms/" + this_day + ".txt";
    while (!writeFile(full_path, ""))
        ;

    uint8_t prev_index = 0;
    uint8_t curr_index = alarm_string.indexOf(',');
    while (curr_index > 0)
    {
        String alarm = alarm_string.substring(prev_index, curr_index);
#ifdef USING_NODEMCU
        Serial.println(alarm);
#endif

        while (!appendFile(full_path, alarm))
            ;

        prev_index = curr_index + 1;
        if (prev_index >= alarm_string.length())
        {
            break;
        }

        curr_index = alarm_string.indexOf(',', prev_index);
    }
}

void updateAlarms(void)
{
    if (has_logged_on)
    {
        String t_day = server.arg("Day");
        String alarms = server.arg("Alarms");

#ifdef USING_NODEMCU
        Serial.println(t_day);
        Serial.println(alarms);
#endif

        if (t_day == NULL)
        {
            server.send(206, "text/plain", "false");
        }
        else
        {
            if (t_day == "all")
            {
                light_updateDayAlarms("sun", alarms);
                light_updateDayAlarms("mon", alarms);
                light_updateDayAlarms("tue", alarms);
                light_updateDayAlarms("wed", alarms);
                light_updateDayAlarms("thu", alarms);
                light_updateDayAlarms("fri", alarms);
                light_updateDayAlarms("sat", alarms);
            }
            else
            {
                light_updateDayAlarms(t_day, alarms);
            }
            server_command = GET_ALARMS;
            server.send(200, "text/plain", "true");
        }
    }
    else
    {
        server.send(401, "text/plain", "Forbiden");
    }
}

bool light_serveAlarms(LIGHT_ALARM alarms[], String day)
{
    // String *table[LIGHT_MAX_ALARMS] = alarms;
    // String *table = alarms;
    String path = "/alarms/" + day + ".txt";
    File file = LittleFS.open(path, "r");
#ifdef USING_NODEMCU
    Serial.println(path);
#endif
    if (!file)
    {
        return false;
    }

    uint8_t pd = 0;
    while (file.available())
    {
        char t_c;
        char t_s[LIGHT_ALARM_SIZE] = "";
        while (file.available())
        {
            t_c = file.read();
            if (t_c == '\n')
            {
                break;
            }
            sprintf(t_s, "%s%c", t_s, t_c);
        }

        if (pd < LIGHT_MAX_ALARMS)
        {
            alarms[pd].hrs = strtol(t_s, NULL, 10);
            alarms[pd].mins = strtol(t_s + 3, NULL, 10);
#ifdef USING_NODEMCU
            Serial.print(alarms[pd].hrs);
            Serial.print(":");
            Serial.println(alarms[pd].mins);
#endif
            pd++;
        }
        else
        {
            break;
        }

        memset(t_s, '\0', LIGHT_ALARM_SIZE);
    }

    file.close();
#ifdef USING_NODEMCU
    Serial.print("Read ");
    Serial.print(pd);
    Serial.println(" alarms");
#endif
    return true;
}

bool light_saveConfigurations(void)
{
    char config[LIGHT_MAX_STR_LEN * 4] = "";
    sprintf(config, "#\n%s\n%s\n%s\n%s\n%u\n%s\n", 
            light_params.whatsapp_number,
            light_params.wifi_name,
            light_params.wifi_password,
            (light_params.light_alarm_on ? "on" : "off"),
            light_params.update_interval,
            (light_params.server_running ? "on" : "off"));

#ifdef USING_NODEMCU
    Serial.print("saved: ");
    Serial.println(config);
#endif

    while (!writeFile("config.txt", config))
        ;

    return true;
}

void light_ResetDefaults(void)
{
#ifdef USING_NODEMCU
    Serial.println("Loading defaults");
#endif

    memset(light_params.whatsapp_number, '\0', LIGHT_MAX_STR_LEN);
    memset(light_params.wifi_password, '\0', LIGHT_MAX_STR_LEN);
    memset(light_params.wifi_name, '\0', LIGHT_MAX_STR_LEN);
    sprintf(light_params.whatsapp_number, "256784356760");
    sprintf(light_params.wifi_password, "Mwanje12");
    sprintf(light_params.wifi_name, "Tech crunch link");
    light_params.light_alarm_on = false;
    light_params.server_running = false;
    light_params.update_interval = 10;

    light_saveConfigurations();

    light_updateDayAlarms("sun", "");
    light_updateDayAlarms("mon", "");
    light_updateDayAlarms("tue", "");
    light_updateDayAlarms("wed", "");
    light_updateDayAlarms("thu", "");
    light_updateDayAlarms("fri", "");
    light_updateDayAlarms("sat", "");

#ifdef USING_NODEMCU
    Serial.println(light_params.whatsapp_number);
    Serial.println(light_params.wifi_name);
    Serial.println(light_params.wifi_password);
    Serial.println(light_params.light_alarm_on);
    Serial.println(light_params.update_interval);
#endif
}

void readConfigurations(void)
{
    File file = LittleFS.open("config.txt", "r");
    if (!file)
    {
#ifdef USING_NODEMCU
        Serial.println("config.txt failed");
#endif

        server_command = ERROR_COMMAND;
        return;
    }
#ifdef USING_NODEMCU
    Serial.println("config.txt opened");
#endif

    if (file.read() != '#')
    {
        file.close();
        light_ResetDefaults();
        return;
    }

    char t_c;
    int cnt = 0;
    char t_s[LIGHT_MAX_STR_LEN] = "";
    while (file.available())
    {
        t_c = file.read();
        if (t_c == '\n')
        {
#ifdef USING_NODEMCU
            Serial.print(cnt); Serial.print(": ");Serial.println(t_s);
#endif

            if (cnt == 1)
            {
                memset(light_params.whatsapp_number, '\0', LIGHT_MAX_STR_LEN);
                sprintf(light_params.whatsapp_number, "%s", t_s);
            }
            else if (cnt == 2)
            {
                memset(light_params.wifi_name, '\0', LIGHT_MAX_STR_LEN);
                sprintf(light_params.wifi_name, "%s", t_s);
            }
            else if (cnt == 3)
            {
                memset(light_params.wifi_password, '\0', LIGHT_MAX_STR_LEN);
                sprintf(light_params.wifi_password, "%s", t_s);
            }
            else if (cnt == 4)
            {
                light_params.light_alarm_on = (strstr(t_s, "off")) ? false : true;
            }
            else if (cnt == 5)
            {
                light_params.update_interval = strtol(t_s, NULL, 10);
            }
            else if (cnt == 6)
            {
                light_params.server_running = (strstr(t_s, "off")) ? false : true;
            }

            cnt++;
            memset(t_s, '\0', LIGHT_MAX_STR_LEN);
            continue;
        }

        sprintf(t_s, "%s%c", t_s, t_c);
    }

    file.close();

#ifdef USING_NODEMCU
    Serial.println(light_params.whatsapp_number);
    Serial.println(light_params.wifi_name);
    Serial.println(light_params.wifi_password);
    Serial.println(light_params.light_alarm_on);
    Serial.println(light_params.update_interval);
#endif
}

void getConfigurations(void)
{
    String res = "{\"number\":\"" + String(light_params.whatsapp_number) + "\",";
    res += "\"institute\":\"" + String(light_params.wifi_name) + "\",";
    res += "\"duration\":" + String(light_params.update_interval) + ",";
    res += "\"alarm\":" + String(light_params.light_alarm_on) + "}";

#ifdef USING_NODEMCU
    Serial.println(res);
#endif

    server.send(200, "text/plain", res);
}

void updateConfigurations(void)
{
#ifdef USING_NODEMCU
    Serial.println(server.args());
    Serial.println(server.arg("Number"));
    Serial.println(server.arg("Name"));
    Serial.println(server.arg("Password"));
    Serial.println(server.arg("Alarms"));
    Serial.println(server.arg("Ringduration"));
#endif
    if (has_logged_on)
    {
        if (server.arg("Number") != NULL)
        {
            memset(light_params.whatsapp_number, '\0', LIGHT_MAX_STR_LEN);
            sprintf(light_params.whatsapp_number, "%s", server.arg("Number").c_str());
        }

        if (server.arg("Name") != NULL)
        {
            memset(light_params.wifi_name, '\0', LIGHT_MAX_STR_LEN);
            sprintf(light_params.wifi_name, "%s", server.arg("Name").c_str());
        }

        if (server.arg("Alarms") == "off")
        {
            light_params.light_alarm_on = false;
        }
        else
        {
            light_params.light_alarm_on = true;
        }

        light_params.update_interval = server.arg("Ringduration").toInt();

        if (server.arg("Password") != NULL)
        {
            memset(light_params.wifi_password, '\0', LIGHT_MAX_STR_LEN);
            sprintf(light_params.wifi_password, "%s", server.arg("Password").c_str());
        }

#ifdef USING_NODEMCU
        Serial.println(light_params.whatsapp_number);
        Serial.println(light_params.wifi_name);
        Serial.println(light_params.wifi_password);
        Serial.println(light_params.light_alarm_on);
        Serial.println(light_params.update_interval);
#endif

        // light_timeSet(server.arg("Datetime"));
        light_saveConfigurations();

        server_command = GET_ALARMS;
        server.send(200, "text/plain", "true");
    }
    else
    {
        server.send(401, "text/plain", "Forbiden");
    }
}

void WIFI_init()
{
    WiFi.forceSleepWake();
    WiFi.mode(WIFI_STA);
    WiFi.begin(light_params.wifi_name, light_params.wifi_password);

#ifdef USING_NODEMCU
    Serial.print("Connecting to ");
    Serial.println(light_params.wifi_name);
#endif

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
#ifdef USING_NODEMCU
        Serial.println(".");
#endif
        light_displayUpdate();
    }

#ifdef USING_NODEMCU
    Serial.println();
    Serial.println("Connected successfully");
#endif
}

bool light_serverStop(void)
{
    server.stop();
    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);
    WiFi.forceSleepBegin();
    delay(1);

    light_params.server_running = false;

    return true;
}

bool light_serverStart(void)
{
    WIFI_init();
    MDNS.begin("autohome");            // Start the mDNS responder for Ahome.local

    server.on("/getstatus", HTTP_GET, serveUpdates);
    server.on("/switchlight", HTTP_GET, toggleLight);
    server.on("/login", HTTP_POST, loginClient);
    server.on("/getalarms", HTTP_GET, getAlarms);
    server.on("/setalarms", HTTP_POST, updateAlarms);
    server.on("/getconfig", HTTP_GET, getConfigurations);
    server.on("/setconfig", HTTP_POST, updateConfigurations);

    server.onNotFound([]() {                                  // If the client requests any URI
        if (!handleFileRequests(server.uri()))                // send it if it exists
            server.send(404, "text/plain", "404: Not Found"); // otherwise, respond with a 404 (Not Found) error
    });

    server.begin();

    light_params.server_running = true;
    // IPAddress ip = WiFi.softAPIP();
    IPAddress ip = WiFi.localIP();
    sprintf(light_params.server_address, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);

#ifdef USING_NODEMCU
    Serial.println(light_params.server_address);
    Serial.print("IP "); Serial.println(WiFi.localIP());
#endif

    return true;
}

void light_serverInit(void)
{
    // LittleFS.format();
    LittleFS.begin();
#ifdef USING_NODEMCU
    Serial.println("server init");
#endif

    readConfigurations();

    light_serverStart();
    // if (light_params.server_running)
    // {
    //     light_serverStart();
    // }
    // else
    // {
    //     light_serverStop();
    // }
}

LIGHT_COMMAND light_serverGetUpdates()
{
    server.handleClient();

    if (millis() - client_millis > CLIENT_TIMEOUT_MINS * 3600)
    {
        has_logged_on = false;
    }

    LIGHT_COMMAND new_command = server_command;
    server_command = NO_COMMAND;

    return new_command;
}

/* ----------------------------------- EOF ---------------------------------- */
