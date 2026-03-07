#ifndef SATELLITE_XCVR
#define SATELLITE_XCVR

#include "transceiver.h"

typedef enum {
    SAT_XCVR_ERR_OK,
    SAT_XCVR_ERR_ERROR
} SAT_XCVR_ERR_e;

SAT_XCVR_ERR_e SAT_XCVR_processCommand(data_packet_s *packet);

#endif