#ifndef TRANSCEIVER
#define TRANSCEIVER

#define ADF7030_1_INSTANCE_NUM 1

#include <stdint.h>




typedef enum {
    TRANSCEIVER_ERR_ERROR,
    TRANSCEIVER_ERR_OK
} TRANSCEIVER_ERR_e;

TRANSCEIVER_ERR_e transceiver_init(void);

TRANSCEIVER_ERR_e ADF7030_1_loadCalibration(void);
TRANSCEIVER_ERR_e ADF7030_1_getTemperature(floatt *temp);
TRANSCEIVER_ERR_e ADF7030_1_transmitPacket(uint8_t *packet);
TRANSCEIVER_ERR_e ADF7030_1_receivePacket(uint8_t *packet);


#ifdef LOAD_CONFIG
void ADF7030_1_loadConfiguration(void);
#endif

#ifdef CALIBRATE
void ADF7030_1_calibrate(void);
#endif

#endif