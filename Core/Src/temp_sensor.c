#include "temp_sensor.h"

#define VOLTAGE_REF     3.3f
#define ADC_RESOLUTION  4095.0f
#define V_OFFSET        0.4f
#define TEMP_COEFF      0.0195f
#define T_INFLECTION    0.0f

float temp_sensor_ADCToTemperature(uint32_t raw_value) {

    float voltage = (float)raw_value * (VOLTAGE_REF / ADC_RESOLUTION);
    return (voltage - V_OFFSET) / TEMP_COEFF + T_INFLECTION;

}