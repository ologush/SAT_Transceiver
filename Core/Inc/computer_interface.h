#ifndef COMPUUTER_INTERFACE
#define COMPUTER_INTERFACE

typedef enum {
    CI_ERR_OK,
    CI_ERR_ERROR,
} CI_ERR_e;

CI_ERR_e CI_Init(void);
CI_ERR_e CI_processCommand(void);
CI_ERR_e CI_dataReady(void);

#endif