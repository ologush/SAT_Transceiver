#include "potentiometer.h"

#define VOLTAGE_REF     3.3f
#define ADC_RESOLUTION  4095.0f

static ADC_HandleTypeDef *hadc;

POTENTIOMETER_ERR_e potentiometer_init(ADC_HandleTypeDef *hadc_ptr) {
    hadc = hadc_ptr;

    return POTENTIOMETER_ERR_OK;
}

POTENTIOMETER_ERR_e potentiometer_getRawValue(uint32_t *value) {
    HAL_ADC_PollForConversion(hadc, HAL_MAX_DELAY);
    *value = HAL_ADC_GetValue(hadc);

    return POTENTIOMETER_ERR_OK;
}

POTENTIOMETER_ERR_e potentiometer_getVoltage(float *voltage) {
    uint32_t raw_value;
    potentiometer_getRawValue(&raw_value);

    *voltage = (float)raw_value * (VOLTAGE_REF / ADC_RESOLUTION);

    return POTENTIOMETER_ERR_OK;
}

POTENTIOMETER_ERR_e potentiometer_getPercentage(float *percentage) {
    float voltage;
    potentiometer_getVoltage(&voltage);

    *percentage = voltage / VOLTAGE_REF;

    return POTENTIOMETER_ERR_OK;
}