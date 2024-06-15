
#include "patient_config.h"
// #include <RTClib.h>

#define CURRENT_YEAR    24

const char daysOfTheWeek[7][10] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
const char monthsOfTheYear[12][4] = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};

// RTC_DS1307 rtc;
bool rtcFound;

bool device_timeInit( void )
{
    // if (!rtc.begin())
    // {
    //     #ifdef DEVICE_DEBUG
    //         Serial.println("RTC not found");
    //     #endif
    //     rtcFound = false;
    //     return false;
    // }

    // if (!rtc.isrunning())
    // {
    //     #ifdef DEVICE_DEBUG
    //         Serial.println("Default time set");
    //     #endif
    //     //rtc not running, let'sadjust the time
    //     rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // }

    rtcFound = true;
    return true;
}

bool device_timeGet( void )
{
    // if(rtcFound)
    // {
    //     DateTime now = rtc.now();

    //     sprintf(device_params.current_date, "%s %s %02u %u",daysOfTheWeek[now.dayOfTheWeek()], monthsOfTheYear[now.month() - 1], now.day(), now.year());
    //     sprintf(device_params.current_time, "%02u:%02u:%02u", now.hour(), now.minute(), now.second());
    // }
    // else
    // {
        sprintf(device_params.current_date, "Mon Feb 06 2024");
        sprintf(device_params.current_time, "13:15:47");
    // }
    // return rtcFound;
    return true;
}

bool device_timeSet(String dts)
{
    // String str = "";
    // dts.toUpperCase();
    // str = dts.substring(0,3);
    // uint8_t mts = 0;

    // if(str == "JAN") { mts = 1;}
    // else if(str == "FEB") { mts = 2;}
    // else if(str == "MAR") { mts = 3;}
    // else if(str == "APR") { mts = 4;}
    // else if(str == "MAY") { mts = 5;}
    // else if(str == "JUN") { mts = 6;}
    // else if(str == "JUL") { mts = 7;}
    // else if(str == "AUG") { mts = 8;}
    // else if(str == "SEP") { mts = 9;}
    // else if(str == "OCT") { mts = 10;}
    // else if(str == "NOV") { mts = 11;}
    // else if(str == "DEC") { mts = 12;}

    // str = dts.substring(4,6);
    // uint8_t dms = str.toInt();

    // str = dts.substring(7,11);
    // uint16_t yrs = str.toInt();

    // str = dts.substring(12,14);
    // uint8_t hrs = str.toInt();

    // str = dts.substring(15,17);
    // uint8_t mns = str.toInt();
  
    // str = dts.substring(18);
    // uint8_t scs = str.toInt();
    
    // if(yrs < CURRENT_YEAR) return false;

    // rtc.adjust(DateTime(yrs, mts, dms, hrs, mns, scs)); // January 21, 2014 at 3am you would call: rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
    return true;
}

uint8_t device_timeGetDay( void )
{
    // DateTime now = rtc.now();

    // return now.dayOfTheWeek();
    return 1;
}

uint8_t device_timeGetHour( void )
{
    // DateTime now = rtc.now();

    // return now.hour();
    return 12;
}

uint8_t device_timeGetMin( void )
{
    // DateTime now = rtc.now();

    // return now.minute();
    return 32;
}

/* ------------------------------- END OF FILE ------------------------------ */
