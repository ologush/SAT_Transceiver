#ifndef TRANSCEIVER
#define TRANSCEIVER

#define ADF7030_1_INSTANCE_NUM 1

#include <stdint.h>


typedef enum {
    TRANSCEIVER_ERR_ERROR,
    TRANSCEIVER_ERR_OK
} TRANSCEIVER_ERR_e;

typedef struct {
    uint8_t header;
    uint8_t length;
    uint8_t payload[128];
} data_packet_s;

TRANSCEIVER_ERR_e transceiver_init(void);

TRANSCEIVER_ERR_e ADF7030_1_loadCalibration(void);
TRANSCEIVER_ERR_e ADF7030_1_getTemperature(float *temp);
TRANSCEIVER_ERR_e ADF7030_1_transmitPacket(uint8_t *packet);
TRANSCEIVER_ERR_e ADF7030_1_receivePacket(uint8_t *packet);


#ifdef LOAD_CONFIG
void ADF7030_1_loadConfiguration(void);
#endif

#ifdef CALIBRATE
void ADF7030_1_calibrate(void);
#endif

#endif