#ifndef POTETNIOMETER
#define POTETNIOMETER

#include "adc.h"

typedef enum {
    POTENTIOMETER_ERR_ERROR,
    POTENTIOMETER_ERR_OK
} POTENTIOMETER_ERR_e;

POTENTIOMETER_ERR_e potentiometer_init(ADC_HandleTypeDef *hadc_ptr);
POTENTIOMETER_ERR_e potentiometer_getRawValue(uint32_t *value);
POTENTIOMETER_ERR_e potentiometer_getVoltage(float *voltage);
POTENTIOMETER_ERR_e potentiometer_getPercentage(float *percentage);

#endif