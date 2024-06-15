
#include "patient_config.h"

/* -------------------------- INCLUDES FOR OXIMETER ------------------------- */
#include <Wire.h>
#include <MAX30105.h>
#include <spo2_algorithm.h>

/* -------------------------- INCLUDES FOR DS18B20 -------------------------- */
#include <OneWire.h>
#include <DallasTemperature.h>

/* -------------------------- DEFINES FOR DS18B20 -------------------------- */
#define ONE_WIRE_BUS        12
#define TEMP_READ_INTERVAL  5

/* -------------------------- DEFINES FOR OXIMETER -------------------------- */
// #define SCL_PIN          10
// #define SDA_PIN          9
#define BUFFER_LENGTH       60
#define PULSE_FACTOR        0.377952756

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature ds18b20(&oneWire);
MAX30105 oximeter;

typedef enum OXIMETER_STATES
{
  COLLECT60,
  ENTER_DUMP15,
  DUMP15,
  ENTER_COLLECT15,
  COLLECT15
} OXIMETER_STATES;
OXIMETER_STATES current_state;

// int16_t temp;
int32_t spo2;
int32_t bpm;
int32_t pulse;
int8_t valid_spo2;
int8_t valid_pulse;
uint8_t samples_count = 0;
unsigned long tempMillis = 0;

uint32_t irBuffer[BUFFER_LENGTH];
uint32_t redBuffer[BUFFER_LENGTH];

uint16_t getTemperature();
SENSOR_STATUS get_sensorSetup();
SENSOR_STATUS get_sensorValues();

bool device_inputSetup(void)
{
  // init temp sensor
  pinMode(ONE_WIRE_BUS, FUNCTION_3);
  ds18b20.begin();

  // init oximeter
  if (sensorSetup() != SENSOR_READY)
  {
#ifdef DEVICE_DEBUG
    Serial.println("Kyiganye!");
#endif
    device_params.device_error = INPUT_ERROR;
    sprintf(device_params.error_message, "Failed to init oximeter");
    return false;
  }

#ifdef DEVICE_DEBUG
  Serial.println("Kakola!");
#endif

  return true;
}

bool device_inputRead(void)
{
  if((millis() - tempMillis) > (TEMP_READ_INTERVAL*1000))
  {
    uint16_t temp = getTemperature();
    if (temp < 200 && temp > 0)
    {
      device_params.temperature = temp;
    }
    else
    {
      device_params.temperature = 0;
    }

    tempMillis = millis();
  }

  // read oximeter
  if (get_sensorValues() == SENSOR_VALUES_READY)
  {
#ifdef DEVICE_DEBUG
    Serial.print("\nP: ");
    Serial.println(bpm);
    Serial.print("S: ");
    Serial.println(spo2);
#endif

    if (bpm < 255 && bpm > 20)
    {
      device_params.heart_rate = bpm;
    }
    else
    {
      device_params.heart_rate = 0;
    }
    if (spo2 < 255 && spo2 > 20)
    {
      device_params.oxygen_level = spo2;
    }
    else
    {
      device_params.oxygen_level = 0;
    }

    return true;
  }

  return false;
}

uint16_t getTemperature()
{
  ds18b20.requestTemperatures();
  float temp = ds18b20.getTempCByIndex(0);

#ifdef DEVICE_DEBUG
  Serial.print("\nTemp: ");
  Serial.println(temp);
#endif

  return (uint16_t)temp;
}

SENSOR_STATUS sensorSetup()
{
  if (!oximeter.begin(Wire, I2C_SPEED_FAST))
  {
    return SENSOR_ERROR;
  }
  oximeter.setup(/*ledBrightness*/ 60, /*sampleAverage*/ 4, /*LEDMode*/ 2, /*sampleRate*/ BUFFER_LENGTH, /*pulseWidth*/ 411, /*ADCRange*/ 4096);

  current_state = COLLECT60;

  return SENSOR_READY;
}

/**
 * @brief -
 * @return - False if the finger was not placed in the oximeter otherwise true
 */
SENSOR_STATUS get_sensorValues()
{

  switch (current_state)
  {
  case COLLECT60:
    oximeter.check(); // check for new data
    if (oximeter.available() == false)
    {
      return SENSOR_NO_FINGER;
    }
    redBuffer[samples_count] = oximeter.getRed();
    irBuffer[samples_count] = oximeter.getIR();
    oximeter.nextSample();
    samples_count++;

    if (samples_count >= BUFFER_LENGTH)
    {
      maxim_heart_rate_and_oxygen_saturation(irBuffer, BUFFER_LENGTH, redBuffer, &spo2, &valid_spo2, &pulse, &valid_pulse);
      bpm = pulse * PULSE_FACTOR;
      current_state = ENTER_DUMP15;
      return SENSOR_VALUES_READY;
    }
    break;

  case ENTER_DUMP15:
    //
    samples_count = 15;
    current_state = DUMP15;
    break;

  case DUMP15:
    //
    if (samples_count < BUFFER_LENGTH)
    {
      redBuffer[samples_count - 15] = redBuffer[samples_count];
      irBuffer[samples_count - 15] = irBuffer[samples_count];
      samples_count++;
    }
    else
    {
      current_state = ENTER_COLLECT15;
    }
    break;

  case ENTER_COLLECT15:
    // coll
    samples_count = 45;
    current_state = COLLECT15;
    break;

  case COLLECT15:
    // coll
    oximeter.check(); // check for new data
    if (oximeter.available() == false)
    {
      return SENSOR_NO_FINGER;
    }
    redBuffer[samples_count] = oximeter.getRed();
    irBuffer[samples_count] = oximeter.getIR();
    oximeter.nextSample();
    samples_count++;

    if (samples_count >= BUFFER_LENGTH)
    {
      maxim_heart_rate_and_oxygen_saturation(irBuffer, BUFFER_LENGTH, redBuffer, &spo2, &valid_spo2, &pulse, &valid_pulse);
      bpm = pulse * PULSE_FACTOR;
      current_state = ENTER_DUMP15;
      return SENSOR_VALUES_READY;
    }
    break;
  }
  return SENSOR_BUSY;
}
