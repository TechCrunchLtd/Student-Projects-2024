
#include "device_config.h"
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <LittleFS.h>

#define DEVICE_USRNAME "admin"
#define DEVICE_PASSWORD "admin"
#define CLIENT_TIMEOUT_MINS     5
#define SERVER_WAIT_SECS        10

ESP8266WebServer server(80);

bool serverWait = false;

unsigned long client_millis;
unsigned long server_millis;

bool appendFile(String dir, String text)
{
    File file = LittleFS.open(dir, "a");

    if (!file)
    {
        device_setError(FILE_ERROR);
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
        device_setError(FILE_ERROR);
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
        serverWait = true;
        server_millis = millis();
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
    res += "\"volt\":" + String(device_params.line_voltage) + ",";
    res += "\"curr\":" + String(device_params.line_current) + ",";
    res += "\"fault\":" + String(device_params.line_fault) + ",";
    res += "\"line\":" + String(device_params.line_status) + "}";

#ifdef DEVICE_DEBUG
    Serial.println(res);
#endif

    server.send(200, "text/plain", res);
}

void serveLogs()
{
    File file = LittleFS.open("line_faults.txt", "r");
    server.streamFile(file, "text");
    file.close();
}

bool device_saveConfigurations(void)
{
    char config[DEVICE_MAX_STR_LEN * 4] = "";
    sprintf(config, "#\n%s\n%s\n%s\n%s\n%s\n%d\n",
            device_params.user_number,
            device_params.server_name,
            device_params.server_password,
            (device_params.line_status ? "on" : "off"),
            (device_params.server_running ? "on" : "off"),
            device_params.log_count);

#ifdef DEVICE_DEBUG
    Serial.print("saved: ");
    Serial.println(config);
#endif
    int cnt=0;
    while (!writeFile("config.txt", config)){
        delay(10);
        if(cnt++ >= 10){
            if(device_setError(FILE_ERROR))
                sprintf(device_params.error_message, "File write failed!");
        }
    }

    return true;
}

void device_ResetDefaults(void)
{
#ifdef DEVICE_DEBUG
    Serial.println("Loading defaults");
#endif

    memset(device_params.server_password, '\0', DEVICE_MAX_STR_LEN);
    memset(device_params.server_name, '\0', DEVICE_MAX_STR_LEN);
    memset(device_params.user_number, '\0', DEVICE_MAX_STR_LEN);
    sprintf(device_params.server_password, "admin123");
    sprintf(device_params.server_name, "Line_monitor");
    sprintf(device_params.user_number, "+256782912385");
    device_params.line_status = false;
    device_params.server_running = true;
    device_params.log_count = 0;

    device_saveConfigurations();

#ifdef DEVICE_DEBUG
    Serial.println(device_params.user_number);
    Serial.println(device_params.server_name);
    Serial.println(device_params.server_password);
#endif
}

bool readConfigurations(void)
{
    File file = LittleFS.open("config.txt", "r");
    if (!file)
    {
#ifdef DEVICE_DEBUG
        Serial.println("config.txt failed");
#endif

        if(device_setError(FILE_ERROR))
            sprintf(device_params.error_message, "Failed to read file!");
        return false;
    }
#ifdef DEVICE_DEBUG
    Serial.println("config.txt opened");
#endif

    if (file.read() != '#')
    {
        file.close();
        device_ResetDefaults();
        return true;
    }

    char t_c;
    int lcnt = 0;
    char t_s[DEVICE_MAX_STR_LEN] = "";
    while (file.available())
    {
        t_c = file.read();
        if (t_c == '\n')
        {
#ifdef DEVICE_DEBUG
            Serial.print(lcnt); Serial.print(": ");Serial.println(t_s);
#endif

            if (lcnt == 1)
            {
                memset(device_params.user_number, '\0', DEVICE_MAX_STR_LEN);
                sprintf(device_params.user_number, "%s", t_s);
            }
            else if (lcnt == 2)
            {
                memset(device_params.server_name, '\0', DEVICE_MAX_STR_LEN);
                sprintf(device_params.server_name, "%s", t_s);
            }
            else if (lcnt == 3)
            {
                memset(device_params.server_password, '\0', DEVICE_MAX_STR_LEN);
                sprintf(device_params.server_password, "%s", t_s);
            }
            else if (lcnt == 4)
            {
                device_params.line_status = (strstr(t_s, "off")) ? false : true;
            }
            else if (lcnt == 5)
            {
                device_params.server_running = (strstr(t_s, "off")) ? false : true;
            }
            else if(lcnt == 6)
            {
                device_params.log_count = strtol(t_s, NULL, 10);
                device_params.log_count++;
            }

            lcnt++;
            memset(t_s, '\0', DEVICE_MAX_STR_LEN);
            continue;
        }

        sprintf(t_s, "%s%c", t_s, t_c);
    }

    file.close();

    if(lcnt == 0)
    {
     #ifdef DEVICE_DEBUG
        Serial.print("no data restoring..");
    #endif
        device_ResetDefaults();
    }

#ifdef DEVICE_DEBUG
    Serial.println("----------configurations-----------");
    Serial.println(device_params.user_number);
    Serial.println(device_params.server_name);
    Serial.println(device_params.server_password);
    Serial.println(device_params.line_status);
    Serial.println(device_params.server_running);
    Serial.println(device_params.log_count);
    Serial.println("----------configurations-----------");
#endif

    return true;
}

void getConfigurations(void)
{
    String res = "{\"user\":\"" + String(device_params.user_number) + "\",";
    res += "\"wifi\":\"" + String(device_params.server_name) + "\"}";

#ifdef DEVICE_DEBUG
    Serial.println(res);
#endif
    server.send(200, "text/plain", res);
}

void updateConfigurations(void)
{
#ifdef DEVICE_DEBUG
    Serial.println(server.args());
    Serial.println(server.arg("User"));
    Serial.println(server.arg("Wifi"));
    Serial.println(server.arg("Password"));
#endif
        if (server.arg("User") != NULL)
        {
            memset(device_params.user_number, '\0', DEVICE_MAX_STR_LEN);
            sprintf(device_params.user_number, "%s", server.arg("User").c_str());
        }

        if (server.arg("Wifi") != NULL)
        {
            memset(device_params.server_name, '\0', DEVICE_MAX_STR_LEN);
            sprintf(device_params.server_name, "%s", server.arg("Wifi").c_str());
        }

        if (server.arg("Password") != NULL)
        {
            memset(device_params.server_password, '\0', DEVICE_MAX_STR_LEN);
            sprintf(device_params.server_password, "%s", server.arg("Password").c_str());
        }

#ifdef DEVICE_DEBUG
        Serial.println(device_params.user_number);
        Serial.println(device_params.server_name);
        Serial.println(device_params.server_password);
#endif

        device_saveConfigurations();

        server.send(200, "text/plain", "true");
}

bool device_serverLogFault(char *info)
{
    unsigned long ftime = millis() / 1000;
    String log_msg = "d" + String(device_params.log_count) + ":" + String(ftime/60) + ":"  + String(ftime%60) + ", ";
    log_msg += String(info);

    device_saveConfigurations();

    #ifdef DEVICE_DEBUG
        Serial.println(log_msg);
    #endif

    return appendFile("line_faults.txt", log_msg);
}

bool device_serverStop(void)
{
    server.stop();
    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);
    WiFi.forceSleepBegin();
    delay(1);

    device_params.server_running = false;

    return false;
}

bool device_serverStart(void)
{

    if(strlen(device_params.server_name) < 3)
    {
        if(device_setError(FILE_ERROR))
            sprintf(device_params.error_message, "Invaalid server name");
        
        device_params.server_running = false;
        return false;
    }

    WiFi.forceSleepWake();
    WiFi.mode(WIFI_STA);
    // WiFi.begin(light_params.wifi_name, light_params.wifi_password);
    WiFi.begin(device_params.server_name, device_params.server_password);

    char conn[100] = {0};
    int waited = 0;
    bool use_hotspot = false;
    sprintf(conn, "Connecting to Wifi");
    while (WiFi.status() != WL_CONNECTED)
    {
        sprintf(conn, "%s.", conn);
        device_displayError(conn);
        delay(1000);
        if(waited++ == 15){
            use_hotspot = true;
            break;
        }
    }

    if(use_hotspot == true)
    {
        WiFi.forceSleepWake();
        WiFi.mode(WIFI_AP);
        WiFi.softAP(device_params.server_name, device_params.server_password);
        device_displayError("Using hotspot");
        delay(1000);
    }

#ifdef DEVICE_DEBUG
    Serial.print("WiFi started!");
#endif

    server.on("/getstatus", HTTP_GET, serveUpdates);
    server.on("/getlogs", HTTP_GET, serveLogs);
    server.on("/getconfig", HTTP_GET, getConfigurations);
    server.on("/setconfig", HTTP_POST, updateConfigurations);

    server.onNotFound([]() {                                  // If the client requests any URI
        if (!handleFileRequests(server.uri()))                // send it if it exists
            server.send(404, "text/plain", "404: Not Found"); // otherwise, respond with a 404 (Not Found) error
    });

    server.begin();

    device_params.server_running = true;
    IPAddress ip = WiFi.localIP();
    sprintf(device_params.server_IP, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);

#ifdef DEVICE_DEBUG
    Serial.print("IP ");
    Serial.println(device_params.server_IP);
    Serial.println(WiFi.localIP());
#endif

    return true;
}

bool device_serverInit(void)
{
    // LittleFS.format();
    LittleFS.begin();
#ifdef DEVICE_DEBUG
    Serial.println("server init");
#endif

    if(!readConfigurations())
        return false;

    device_setError(NO_ERROR);
    return true;
    
}

bool device_serverUpdate()
{
    server.handleClient();
    while(serverWait)
    {
        server.handleClient();

        if(millis() - serverWait > SERVER_WAIT_SECS*1000)
        {
            serverWait = false;
        }
    }

    IPAddress ip = WiFi.localIP();
    sprintf(device_params.server_IP, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);

    return true;
}

/* ----------------------------------- EOF ---------------------------------- */
