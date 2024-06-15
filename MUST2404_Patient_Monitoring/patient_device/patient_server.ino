
#include "patient_config.h"
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

#define LIGHT_SERVER_NAME "autolight"
#define DEVICE_USRNAME "doctor@patient.com"
#define DEVICE_PASSWORD "doctor123"
#define CLIENT_TIMEOUT_MINS     5
#define SERVER_WAIT_SECS        10

ESP8266WebServer server(80);

bool has_logged_on = false;
bool serverWait = false;

unsigned long client_millis;
unsigned long server_millis;

bool appendFile(String dir, String text)
{
    File file = LittleFS.open(dir, "a");

    if (!file)
    {
        device_errorHandler(FILE_ERROR);
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
        device_errorHandler(FILE_ERROR);
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
    res += "\"allowed\":" + String(has_logged_on) + ",";
    res += "\"dev\":\"" + String(device_params.device_id) + "\",";
    res += "\"temp\":" + String(device_params.temperature) + ",";
    res += "\"pulse\":" + String(device_params.heart_rate) + ",";
    res += "\"spo2\":" + String(device_params.oxygen_level) + "}";

#ifdef DEVICE_DEBUG
    Serial.println(res);
#endif

    server.send(200, "text/plain", res);
}

void loginClient(void)
{
#ifdef DEVICE_DEBUG
    Serial.print("Login: ");
    Serial.print(server.arg("Username"));
    Serial.print(" ");
    Serial.println(server.arg("Password"));
#endif

    if (server.arg("Username") == DEVICE_USRNAME && server.arg("Password") == DEVICE_PASSWORD)
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

bool device_saveConfigurations(void)
{
    char config[DEVICE_MAX_STR_LEN * 4] = "";
    sprintf(config, "#\n%s\n%s\n%s\n%s\n%u\n%s\n", 
            device_params.device_id,
            device_params.wifi_name,
            device_params.wifi_password,
            (device_params.updates_enabled ? "on" : "off"),
            device_params.update_interval,
            (device_params.server_running ? "on" : "off"));

#ifdef DEVICE_DEBUG
    Serial.print("saved: ");
    Serial.println(config);
#endif
    int cnt=0;
    while (!writeFile("config.txt", config)){
        delay(10);
        if(cnt++ >= 10){
            device_params.device_error = FILE_ERROR;
            sprintf(device_params.error_message, "File write failed!");
            device_errorHandler(FILE_ERROR);
        }
    }

    return true;
}

void device_ResetDefaults(void)
{
#ifdef DEVICE_DEBUG
    Serial.println("Loading defaults");
#endif

    memset(device_params.wifi_password, '\0', DEVICE_MAX_STR_LEN);
    memset(device_params.wifi_name, '\0', DEVICE_MAX_STR_LEN);
    memset(device_params.device_id, '\0', DEVICE_MAX_STR_LEN);
    sprintf(device_params.wifi_password, "Boffins@13");
    sprintf(device_params.wifi_name, "boffins_home");
    sprintf(device_params.device_id, "pm001");
    device_params.updates_enabled = true;
    device_params.server_running = true;
    device_params.update_interval = 10;

    device_saveConfigurations();

#ifdef DEVICE_DEBUG
    Serial.println(device_params.device_id);
    Serial.println(device_params.wifi_name);
    Serial.println(device_params.wifi_password);
    Serial.println(device_params.updates_enabled);
    Serial.println(device_params.update_interval);
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

        device_params.device_error = FILE_ERROR;
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
    int cnt = 0;
    char t_s[DEVICE_MAX_STR_LEN] = "";
    while (file.available())
    {
        t_c = file.read();
        if (t_c == '\n')
        {
#ifdef DEVICE_DEBUG
            Serial.print(cnt); Serial.print(": ");Serial.println(t_s);
#endif

            if (cnt == 1)
            {
                memset(device_params.device_id, '\0', DEVICE_MAX_STR_LEN);
                sprintf(device_params.device_id, "%s", t_s);
            }
            else if (cnt == 2)
            {
                memset(device_params.wifi_name, '\0', DEVICE_MAX_STR_LEN);
                sprintf(device_params.wifi_name, "%s", t_s);
            }
            else if (cnt == 3)
            {
                memset(device_params.wifi_password, '\0', DEVICE_MAX_STR_LEN);
                sprintf(device_params.wifi_password, "%s", t_s);
            }
            else if (cnt == 4)
            {
                device_params.updates_enabled = (strstr(t_s, "off")) ? false : true;
            }
            else if (cnt == 5)
            {
                device_params.update_interval = strtol(t_s, NULL, 10);
            }
            else if (cnt == 6)
            {
                device_params.server_running = (strstr(t_s, "off")) ? false : true;
            }

            cnt++;
            memset(t_s, '\0', DEVICE_MAX_STR_LEN);
            continue;
        }

        sprintf(t_s, "%s%c", t_s, t_c);
    }

    file.close();

#ifdef DEVICE_DEBUG
    Serial.println(device_params.device_id);
    Serial.println(device_params.wifi_name);
    Serial.println(device_params.wifi_password);
    Serial.println(device_params.updates_enabled);
    Serial.println(device_params.update_interval);
#endif

    return true;
}

void getConfigurations(void)
{
    String res = "{\"device\":\"" + String(device_params.device_id) + "\",";
    res += "\"wifi\":\"" + String(device_params.wifi_name) + "\",";
    res += "\"interval\":" + String(device_params.update_interval) + ",";
    res += "\"update\":" + String(device_params.updates_enabled) + "}";

#ifdef DEVICE_DEBUG
    Serial.println(res);
#endif

    server.send(200, "text/plain", res);
}

void updateConfigurations(void)
{
#ifdef DEVICE_DEBUG
    Serial.println(server.args());
    Serial.println(server.arg("Device"));
    Serial.println(server.arg("Name"));
    Serial.println(server.arg("Password"));
    Serial.println(server.arg("Update"));
    Serial.println(server.arg("Interval"));
#endif
    if (has_logged_on)
    {
        if (server.arg("Device") != NULL)
        {
            memset(device_params.device_id, '\0', DEVICE_MAX_STR_LEN);
            sprintf(device_params.device_id, "%s", server.arg("Device").c_str());
        }

        if (server.arg("Name") != NULL)
        {
            memset(device_params.wifi_name, '\0', DEVICE_MAX_STR_LEN);
            sprintf(device_params.wifi_name, "%s", server.arg("Name").c_str());
        }

        if (server.arg("Update") == "off")
        {
            device_params.updates_enabled = false;
        }
        else
        {
            device_params.updates_enabled = true;
        }

        device_params.update_interval = server.arg("Interval").toInt();

        if (server.arg("Password") != NULL)
        {
            memset(device_params.wifi_password, '\0', DEVICE_MAX_STR_LEN);
            sprintf(device_params.wifi_password, "%s", server.arg("Password").c_str());
        }

#ifdef DEVICE_DEBUG
        Serial.println(device_params.device_id);
        Serial.println(device_params.wifi_name);
        Serial.println(device_params.wifi_password);
        Serial.println(device_params.updates_enabled);
        Serial.println(device_params.update_interval);
#endif

        device_saveConfigurations();

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
    WiFi.begin(device_params.wifi_name, device_params.wifi_password);

#ifdef DEVICE_DEBUG
    Serial.print("Connecting to ");
    Serial.println(device_params.wifi_name);
#endif

    device_params.device_error = COMM_ERROR;
    if(strlen(device_params.wifi_name) < 17){
        sprintf(device_params.error_message, "...%s", device_params.wifi_name);
    }
    else{
        sprintf(device_params.error_message, "Connecting to WiFi..");
    }

    while (WiFi.status() != WL_CONNECTED)
    {
        // delay(1000);
#ifdef DEVICE_DEBUG
        Serial.println(".");
#endif
        device_inputRead();
        device_displayUpdate();
    }

#ifdef DEVICE_DEBUG
    Serial.println();
    Serial.println("Connected successfully");
#endif

    device_params.device_error = NO_ERROR;
}

bool device_serverStop(void)
{
    server.stop();
    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);
    WiFi.forceSleepBegin();
    delay(1);

    device_params.server_running = false;

    return true;
}

bool device_serverStart(void)
{
    WIFI_init();
    MDNS.begin("patients");            // Start the mDNS responder for Ahome.local

    server.on("/getstatus", HTTP_GET, serveUpdates);
    server.on("/login", HTTP_POST, loginClient);
    server.on("/getconfig", HTTP_GET, getConfigurations);
    server.on("/setconfig", HTTP_POST, updateConfigurations);

    server.onNotFound([]() {                                  // If the client requests any URI
        if (!handleFileRequests(server.uri()))                // send it if it exists
            server.send(404, "text/plain", "404: Not Found"); // otherwise, respond with a 404 (Not Found) error
    });

    server.begin();

    device_params.server_running = true;
    // IPAddress ip = WiFi.softAPIP();
    IPAddress ip = WiFi.localIP();
    sprintf(device_params.local_server, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);

#ifdef DEVICE_DEBUG
    Serial.println(device_params.local_server);
    Serial.print("IP "); Serial.println(WiFi.localIP());
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

    device_serverStart();

    return device_onlineInit();
    
}

bool device_serverUpdate()
{
    server.handleClient();
    while(serverWait)
    {
        if(millis() - serverWait > SERVER_WAIT_SECS*1000)
        {
            serverWait = false;
        }
        server.handleClient();
    }

    if (millis() - client_millis > CLIENT_TIMEOUT_MINS * 60000)
    {
        has_logged_on = false;
    }
    return true;
}

/* ----------------------------------- EOF ---------------------------------- */
