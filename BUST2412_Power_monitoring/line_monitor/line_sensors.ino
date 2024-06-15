

#include "line_monitor.h"
#include <ZMPT101B.h>
#include <ACS712.h>

#define VOLTAGE_SENSOR A3
#define CURRENT_SENSOR A2
#define OPERATING_VOLTAGE   220
#define MAXIMUM_VOLTAGE     310
#define MAXIMUM_LOADING     10000
#define SHORT_CCT_CURRENT   20000

ZMPT101B voltageSensor(VOLTAGE_SENSOR);
ACS712 currentSensor(CURRENT_SENSOR, 5.0, 1023, 66);

bool device_SensorInit()
{
    // voltageSensor.calibrate();

    // currentSensor.calibrate();
    
    return true;
}


uint16_t getMaxVolt()
{
	uint16_t volts = 0;
	for( byte cnt=0; cnt<10; cnt++ ) {
		uint16_t new_volt = (((VOLTAGE_SENSOR / 1023.0) * 5.0) * 100.0);
		if( new_volt > volts ) volts = new_volt;
		delay(1);
	}
	return volts;
}

LINE_FAULT device_voltageSensorRead()
{
    float voltage = voltageSensor.getVoltageAC();
    voltage = voltage * (215.06/233.95);
    if(voltage <= (float)MAXIMUM_VOLTAGE)
    {
        line_params.voltage = (uint16_t)(voltage*100);
        if(voltage > (OPERATING_VOLTAGE*115.0/100.0))
            return OVER_VOLTAGE;
        
        if(voltage < (OPERATING_VOLTAGE*85.0/100.0))
            return UNDER_VOLTAGE;
        
    }

    return NO_FAULT;
}


LINE_FAULT device_currentSensorRead()
{
    float current = currentSensor.mA_AC();
    // current = current / 1000.0;

    line_params.current = (uint16_t)(current);
    if(current > (float)SHORT_CCT_CURRENT)
        return SHORT_CIRCUIT;

    else if(current >= (float)MAXIMUM_LOADING)
        return OVER_LOAD;

    return NO_FAULT;
}

/* ----------------------------------- EOF ---------------------------------- */