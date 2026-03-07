#ifndef COMPUUTER_INTERFACE
#define COMPUTER_INTERFACE

#include <stdint.h>

typedef enum {
    CI_ERR_OK,
    CI_ERR_ERROR,
} CI_ERR_e;

CI_ERR_e CI_processCommand(uint8_t *buf, uint16_t len);

#endif