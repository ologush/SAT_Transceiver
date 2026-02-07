#ifndef TEMP_SENSOR
#define TEMP_SENSOR

#include "adc.h"

typedef enum {
    TEMP_SENSOR_ERR_ERROR,
    TEMP_SENSOR_ERR_OK
} TEMP_SENSOR_ERR_e;

TEMP_SENSOR_ERR_e temp_sensor_init(ADC_HandleTypeDef *hadc_ptr);
TEMP_SENSOR_ERR_e temp_sensor_getTemperature(float *temperature);


#endif