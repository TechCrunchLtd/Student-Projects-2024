
#include "home_config.h"

#ifdef USING_NODEMCU
#define LIGHT_PIN 12
#else
#define LIGHT_PIN 1
#endif

#define LIGHT_UPDATE_PERIOD 100
#define LIGHT_RESET_WAIT_SECS 10

bool light_loaded_alarms;

LIGHT_ALARM light_alarmTable[LIGHT_MAX_ALARMS];

unsigned long light_ring_startMillis;
unsigned long light_updateMillis;

LIGHT_PARAMS light_params;

void startactivated()
{
    digitalWrite(LIGHT_PIN, HIGH);
    light_params.light_activated = true;
    light_ring_startMillis = millis();
}

void stopLighting()
{
    digitalWrite(LIGHT_PIN, LOW);
    light_params.light_activated = false;
}

void setInitialState()
{
#ifndef USING_NODEMCU
    pinMode(LIGHT_PIN, FUNCTION_3);
#endif
    pinMode(LIGHT_PIN, OUTPUT);
    stopLighting();
}

void tryThis(void)
{
    while (1)
    {
        digitalWrite(LIGHT_PIN, HIGH);
        delay(1000);
        digitalWrite(LIGHT_PIN, LOW);
        delay(1000);
        while (digitalRead(3))
        {
            delay(500);
        }
    }
}

void light_errorHandler(LIGHT_ERROR new_error)
{
#ifdef USING_NODEMCU
    Serial.println(new_error);
#endif

    light_displayError("ERROR");
    while (1)
    {
        delay(1000);
    }
}

/// @brief get the time the light should activate and save it as the next_alarm
void getNextAlarm(void)
{
    memset(light_params.next_alarm, '\0', LIGHT_ALARM_SIZE);

    if (!light_params.light_alarm_on)
    {
        sprintf(light_params.next_alarm, "OFF");
        return;
    }

    uint8_t hrs = light_timeGetHour();
    uint8_t mins = light_timeGetMin();

    uint8_t hrz = (mins == 59) ? (hrs + 1) % 24 : hrs;
    for (hrz; hrz < 24; hrz++)
    {
        uint8_t minz = ((hrz != hrs) || (mins == 59)) ? 0 : mins + 1;
        for (minz; minz < 60; minz++)
        {
            // char buf[LIGHT_ALARM_SIZE];
            // sprintf(buf, "%02u:%02u", hrz, m);
            for (uint8_t pd = 0; pd < LIGHT_MAX_ALARMS; pd++)
            {
                if ((light_alarmTable[pd].hrs == hrz) && (light_alarmTable[pd].mins == minz))
                {
                    sprintf(light_params.next_alarm, "%02u:%02u", hrz, minz);
#ifdef USING_NODEMCU
                    Serial.print("NEXT: ");
                    Serial.println(light_params.next_alarm);
#endif
                    return;
                }
            }
            yield();
        }
    }

#ifdef USING_NODEMCU
    Serial.println("NO NEXT!");
#endif

    sprintf(light_params.next_alarm, "NONE");
}

/// @brief load today's alarms into the alarm table
/// @note upon succesfully loading the alarms, this function will also load the next alarm by calling the getNextAlarm function
void loadAlarms()
{
#ifdef USING_NODEMCU
    Serial.println("Loading alarms");
#endif

    switch (light_timeGetDay())
    {
    case 0:
        light_loaded_alarms = light_serveAlarms(light_alarmTable, "sun");
        break;
    case 1:
        light_loaded_alarms = light_serveAlarms(light_alarmTable, "mon");
        break;
    case 2:
        light_loaded_alarms = light_serveAlarms(light_alarmTable, "tue");
        break;
    case 3:
        light_loaded_alarms = light_serveAlarms(light_alarmTable, "wed");
        break;
    case 4:
        light_loaded_alarms = light_serveAlarms(light_alarmTable, "thu");
        break;
    case 5:
        light_loaded_alarms = light_serveAlarms(light_alarmTable, "fri");
        break;
    case 6:
        light_loaded_alarms = light_serveAlarms(light_alarmTable, "sat");
        break;
    default:
        light_errorHandler(RTC_ERROR);
    }

    if (light_loaded_alarms)
    {
#ifdef USING_NODEMCU
        for (int i = 0; i < LIGHT_MAX_ALARMS; i++)
        {
            Serial.print(light_alarmTable[i].hrs);
            Serial.print(":");
            Serial.println(light_alarmTable[i].mins);
        }
#endif

        getNextAlarm();
    }
    else
    {
        light_errorHandler(FILE_ERROR);
    }
}

/// @brief Check and process any incoming inputs
/// @note Inputs can be from local hardware, local server or from online
void processInputs()
{
    LIGHT_COMMAND new_command = light_getOnlineRequest();

    if (new_command != NO_COMMAND)
    {
        light_displayRefresh();
    }
    else if (light_params.server_running)
    {
        new_command = light_serverGetUpdates();

        if(new_command == NO_COMMAND){
            new_command = light_inputRead();
        }
    }

#ifdef USING_NODEMCU
    if (new_command != NO_COMMAND)
    {
        Serial.print("CMD: ");
        Serial.println(new_command);
    }
#endif

    switch (new_command)
    {
    case NO_COMMAND:
        // code
        break;
    case LIGHT_SCREEN:
        light_displayRefresh();
        break;
    case GET_ALARMS:
        loadAlarms();
        break;
    case ERROR_COMMAND:
        light_errorHandler(CMD_ERROR);
        break;
    case SWITCH_LIGHT:
        if (light_params.light_activated)
        {
            stopLighting();
        }
        else
        {
            startactivated();
        }
        break;
    case SWITCH_ALARM:
        light_params.light_alarm_on = !light_params.light_alarm_on;
        light_saveConfigurations();
        loadAlarms();
        break;
    case SWITCH_SERVER:
        if (light_params.server_running)
        {
            light_serverStop();
        }
        else
        {
            light_serverStart();
        }
        break;
    case FACTORY_RESET:
        light_displayError("RESET");
        unsigned long now_millis = millis();
        while (millis() - now_millis < LIGHT_RESET_WAIT_SECS * 1000)
        {
            if (light_inputRead() != NO_COMMAND)
            {
                return;
            }
            delay(10);
        }
        light_ResetDefaults();
        ESP.restart(); // restart device
        break;
    }
}

/// @brief check if there a ready alarm toactivate the light and prepare the next alarm
/// @note this function will also load the alarms for the following day when it clocks midnight
void handleAlarms()
{
    if (light_params.light_activated && (light_params.light_duration != 0))
    {
        if ((millis() - light_ring_startMillis) > (light_params.light_duration * 60000))
        {
            stopLighting();
        }

        return;
    }

    if (strstr(light_params.current_time, "23:59") && light_loaded_alarms)
    {
        light_loaded_alarms = false;
    }

    if (strstr(light_params.current_time, "00:00") && !light_loaded_alarms)
    {
        loadAlarms();
    }

    if ((strlen(light_params.next_alarm) == 5) && (strstr(light_params.current_time, light_params.next_alarm)))
    {
#ifdef USING_NODEMCU
        Serial.println(light_params.current_time);
        Serial.print("Time to ring: ");
        Serial.println(light_params.next_alarm);
#endif

        startactivated();
        getNextAlarm();
        light_displayRefresh();
    }
}

/// @brief first things first
void setup()
{
#ifdef USING_NODEMCU
    Serial.begin(115200);
    delay(100);
    Serial.println("Starting...");
#endif
    setInitialState();
    light_inputSetup();
    light_displayInit();
    light_timeInit();
    light_serverInit();
    loadAlarms();
    light_onlineInit();
#ifdef USING_NODEMCU
    Serial.println("Set up complete");
#endif
}

/// @brief keep it going
void loop()
{
    processInputs();

    if (millis() - light_updateMillis > LIGHT_UPDATE_PERIOD)
    {
        // light_timeGet();
        if (light_params.light_alarm_on == true)
        {
            handleAlarms();
        }
        light_displayUpdate();
        light_updateMillis = millis();
    }

//     if (light_params.light_activated && (light_params.light_duration != 0) && (millis() - light_ring_startMillis > light_params.light_duration * 60000))
//     {
// #ifdef USING_NODEMCU
//         Serial.println("stop light");
// #endif

//         stopLighting();
//     }
}

/* ----------------------------------- EOF ---------------------------------- */