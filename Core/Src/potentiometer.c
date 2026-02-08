#include "potentiometer.h"

#define VOLTAGE_REF     3.3f
#define ADC_RESOLUTION  4095.0f

float potentiometer_ADCToVoltage(uint32_t raw_value) {

    return (float)raw_value * (VOLTAGE_REF / ADC_RESOLUTION);

}

float potentiometer_ADCToPercentage(uint32_t raw_value) {

    float voltage = potentiometer_ADCToVoltage(raw_value);

    return voltage / VOLTAGE_REF;
}