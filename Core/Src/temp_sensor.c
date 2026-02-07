#include "temp_sensor.h"

#define VOLTAGE_REF     3.3f
#define ADC_RESOLUTION  4095.0f
#define V_OFFSET        0.4f
#define TEMP_COEFF      0.0195f
#define T_INFLECTION    0.0f

static ADC_HandleTypeDef *hadc;

TEMP_SENSOR_ERR_e temp_sensor_init(ADC_HandleTypeDef *hadc_ptr) {

    hadc = hadc_ptr;

    return TEMP_SENSOR_ERR_OK;
}

TEMP_SENSOR_ERR_e temp_sensor_getTemperature(float *temperature) {

    HAL_ADC_PollForConversion(hadc, HAL_MAX_DELAY);
    uint32_t adc_value = HAL_ADC_GetValue(hadc);

    float voltage = (float)adc_value * (VOLTAGE_REF / ADC_RESOLUTION);
    *temperature = (voltage - V_OFFSET) / TEMP_COEFF + T_INFLECTION;

    return TEMP_SENSOR_ERR_OK;
}