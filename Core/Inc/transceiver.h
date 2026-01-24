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

typedef struct {
    SPI_HandleTypeDef *hspi;

    GPIO_TypeDef *cs_port;
    uint16_t cs_pin;

    GPIO_TypeDef *rst_port;
    uint16_t rst_pin;

    GPIO_TypeDef *miso_port;
    uint16_t miso_pin;

    uint32_t spi_timeout;
} ADF7030_s;

enum {
    ADF7030_PHY_SLEEP = 0x00,
    ADF7030_PHY_OFF   = 0x01,
    ADF7030_PHY_ON    = 0x02,
    ADF7030_PHY_RX    = 0x03,
    ADF7030_PHY_TX    = 0x04,
    ADF7030_CFG_DEV   = 0x05
} ADF7030_STATE_MACHINE_e;

TRANSCEIVER_ERR_e ADF7030_init(SPI_HandleTypeDef *hspi, GPIO_TypeDef *cs_port, uint16_t cs_pin, GPIO_TypeDef *rst_port, uint16_t rst_pin, GPIO_TypeDef *miso_port, uint16_t miso_pin);
TRANSCEIVER_ERR_e ADF7030_loadCalibration(void);
TRANSCEIVER_ERR_e ADF7030_getTemperature(float *temp);
TRANSCEIVER_ERR_e ADF7030_transmitPacket(uint8_t *packet);
TRANSCEIVER_ERR_e ADF7030_receivePacket(uint8_t *packet);


#ifdef LOAD_CONFIG
void ADF7030_1_loadConfiguration(void);
#endif

#ifdef CALIBRATE
void ADF7030_1_calibrate(void);
#endif

#endif