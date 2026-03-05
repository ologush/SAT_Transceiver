#ifndef COMPUUTER_INTERFACE
#define COMPUTER_INTERFACE

#include <stdint.h>

typedef enum {
    CI_ERR_OK,
    CI_ERR_ERROR,
} CI_ERR_e;

typedef enum {
    CI_CMD_NONE = 0x00,
    CI_CMD_SET_ADCS_MODE = 0x01,
    CI_CMD_SET_ADCS_TARGET = 0x02,
    CI_CMD_GET_SENSOR_DATA = 0x03,
    CI_RESP_SENSOR_DATA = 0x10
} CI_CMD_e;




CI_ERR_e CI_Init(void);
CI_ERR_e CI_sendCommand(uint8_t *buf, uint16_t len);
CI_ERR_e CI_processCommand(uint8_t *buf, uint16_t len);
CI_ERR_e CI_dataReady(void);

#endif