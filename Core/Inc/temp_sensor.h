#ifndef TEMP_SENSOR
#define TEMP_SENSOR

#include "adc.h"

float temp_sensor_ADCToTemperature(uint32_t raw_value);

#endif