/**
 * @brief Monitor the state of the panel by monitoring the light radiance from the sun
 *          inline with the power being output by the panel
 * 
 * @details Sensors used: voltage, current and LDR
*/

#include "Auto_solar_cleaner.h"

#define LIGHT_SENSOR        A0
#define CURRENT_SENSOR      A2
#define VOLTAGE_SENSOR      A3
#define CURRENT_SENSITIVITY 66
#define LIGHT_INTENSITY     400
#define RATED_POWER_mW      5000

typedef enum MONITOR_STATE
{
    READ_VOLTAGE,
    READ_CURRENT,
    READ_LIGHT,
    COMPARE_POWER
}
MONITOR_STATES;

MONITOR_STATE monitor_state;

int getADCvalue(uint8_t adc_pin)
{
    int advalue = 0;
    for(int i=0; i<10; i++)
    {
        advalue += analogRead(adc_pin);
        delay(1);
    }

    if(advalue == 0)
    {
        Serial.println("Sensor fault");
        return 1;
    }

    return advalue;
}

bool readVoltage(void)
{
    int advalue = 0;
    float R1 = 30000.0;
    float R2 = 7500.0;

    advalue = getADCvalue(VOLTAGE_SENSOR);
    if(advalue == 0)
        return false;

    float volt_ref = (float)advalue * 1.0 /10.0;
    volt_ref = (volt_ref/1023.0) * 5.0;

    volt_ref = volt_ref * (R1+R2)/R2;
    solar_params.voltage = volt_ref;

    return true;
}

bool readCurrent(void)
{
    int advalue = getADCvalue(CURRENT_SENSOR);
    if(advalue == 0)
        return false;

    float curr_ref = (float)advalue * 1.0 / 10.0;
    curr_ref = ((curr_ref/1023.0) * 5000.0) - 2500.0;

    if(curr_ref < 1.0) curr_ref = 0.0;

    solar_params.current = curr_ref * CURRENT_SENSITIVITY;



    return true;
}

bool readLight(void)
{
    int advalue = getADCvalue(LIGHT_SENSOR);
    if(advalue == 0)
        return false;
    
    solar_params.light = advalue/10;

    return true;
}

bool checkPowerDelivery(void)
{
    if(solar_params.light > LIGHT_INTENSITY)
    {
        float power = solar_params.current * solar_params.voltage;
        if(power < (float)RATED_POWER_mW)
        {
            Serial.println("Low power!");
            return false;
        }
    }

    return true;
}

bool monitor_initialise(void)
{
    //init current sensor
    //init voltage sensor

    monitor_state = READ_VOLTAGE;
    solar_params.panel_is_clean = true;

    return true;
}

bool monitor_panelClean(void)
{
    switch(monitor_state)
    {
        case READ_VOLTAGE:
            if(readVoltage() != false)
            {
                monitor_state = READ_CURRENT;
            }
        break;
        
        case READ_CURRENT:
            if(readCurrent() != false)
            {
                monitor_state = READ_LIGHT;
            }
        break;

        case READ_LIGHT:
            if(readLight() != false)
            {
                monitor_state = COMPARE_POWER;
            }
        break;

        case COMPARE_POWER:
            solar_params.panel_is_clean = checkPowerDelivery();
            monitor_state = READ_VOLTAGE;
        break;

    }

    return solar_params.panel_is_clean;
}