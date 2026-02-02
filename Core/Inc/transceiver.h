#ifndef TRANSCEIVER
#define TRANSCEIVER

#define ADF7030_1_INSTANCE_NUM 1
#define TX_PACKET_MEMORY 0x2000AF0
#define RX_PACKET_MEMORY 0x2000CF0

#include <stdint.h>
#include "spi.h"

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

typedef enum {
    ADF7030_PHY_SLEEP = 0x00,
    ADF7030_PHY_OFF   = 0x01,
    ADF7030_PHY_ON    = 0x02,
    ADF7030_PHY_RX    = 0x03,
    ADF7030_PHY_TX    = 0x04,
    ADF7030_CFG_DEV   = 0x05,
    ADF7030_CCA       = 0x06,
    ADF7030_DO_CAL    = 0x09,
    ADF7030_GPCLK     = 0x10,
    ADF7030_MON       = 0x0A,
    ADF7030_LFRC_CAL  = 0x0C
} ADF7030_STATE_e;

typedef enum {
    ADF7030_TRANSITION      = 0x00,
    ADF7030_XQ_IN_STATE     = 0x01,
    ADF7030_IDLE_IN_STATE   = 0x02
} ADF7030_TRANSITION_STATUS_e;

typedef enum {
    ADF7030_MEMORY_CMD = 0x00,
    ADF7030_RADIO_CMD  = 0x01
} ADF7030_CMD_TYPE_e;

typedef enum {
    ADF7030_NOT_READY   = 0x00,
    ADF7030_READY       = 0x01
} ADF7030_READY_STATE_e;

typedef enum {
    ADF7030_MEM_WRITE = 0x00,
    ADF7030_MEM_READ  = 0x01
} ADF7030_MEM_RNW_e;

typedef enum {
    ADF7030_MEM_RANDOM = 0x00,
    ADF7030_MEM_BLOCK  = 0x01
} ADF7030_MEM_BNR_e;

typedef enum {
    ADF7030_MEM_PTR  = 0x00,
    ADF7030_MEM_ADDR = 0x01
} ADF7030_MEM_ANP_e;

typedef enum {
    ADF7030_MEM_SHORT = 0x00,
    ADF7030_MEM_LONG  = 0x01,
} ADF7030_MEM_LNS_e;

typedef enum {
    ADF7030_MEM_PTR_SETUP = 0x00,
    ADF7030_MEM_PTR_0     = 0x05,
    ADF7030_MEM_PTR_1     = 0x06,
    ADF7030_MEM_PTR_2     = 0x07
} ADF7030_PTR_BASE_ADDR_e;

TRANSCEIVER_ERR_e ADF7030_init(SPI_HandleTypeDef *hspi, GPIO_TypeDef *cs_port, uint16_t cs_pin, GPIO_TypeDef *rst_port, uint16_t rst_pin, GPIO_TypeDef *miso_port, uint16_t miso_pin);
TRANSCEIVER_ERR_e ADF7030_loadCalibration(void);
TRANSCEIVER_ERR_e ADF7030_getTemperature(float *temp);
TRANSCEIVER_ERR_e ADF7030_transmitPacket(data_packet_s *packet);
TRANSCEIVER_ERR_e ADF7030_receivePacket(data_packet_s *packet);
TRANSCEIVER_ERR_e ADF7030_transitionState(ADF7030_STATE_e target_state);
ADF7030_STATE_e ADF7030_getState(void);
ADF7030_TRANSITION_STATUS_e ADF7030_getTransitionStatus(void);
ADF7030_READY_STATE_e ADF7030_getReadyState(void);
TRANSCEIVER_ERR_e ADF7030_radioCommand();
TRANSCEIVER_ERR_e ADF7030_memoryCommand(uint32_t address);
TRANSCEIVER_ERR_e ADF7030_getTemperature(float *temp);

TRANSCEIVER_ERR_e ADF7030_memoryWrite(uint32_t address, uint8_t *data, uint32_t nbytes);
TRANSCEIVER_ERR_e ADF7030_memoryRead(uint32_t address, uint8_t *data, uint32_t nbytes);
TRANSCEIVER_ERR_e ADF7030_radioSettings(void);


#ifdef LOAD_CONFIG
void ADF7030_1_loadConfiguration(void);
#endif

#ifdef CALIBRATE
void ADF7030_1_calibrate(void);
#endif

#endif