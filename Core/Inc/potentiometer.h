#ifndef POTETNIOMETER
#define POTETNIOMETER

#include "adc.h"

float potentiometer_ADCToVoltage(uint32_t raw_value);
float potentiometer_ADCToPercentage(uint32_t raw_value);

#endif