#include "computer_interface.h"
#include "transceiver.h"

#include <stdio.h>
#include "usbd_cdc_if.h"
#include <string.h>

extern float current_temperature;
extern float current_potentiometer_percentage;

CI_ERR_e CI_Init(void) {

    return CI_ERR_OK;

}

CI_ERR_e CI_sendCommand(uint8_t *buf, uint16_t len) {

    data_packet_s packet = {
        .length = len,
    };

    if (len > 128) {
        return CI_ERR_ERROR;
    }
    
    memcpy(packet.payload, buf, len);

    ADF7030_transmitPacket(&packet);

    return CI_ERR_OK;

}

CI_ERR_e CI_processCommand(uint8_t *buf, uint16_t len) {

    CI_CMD_e cmd = buf[0];

    switch (cmd) {
        case CI_CMD_SET_ADCS_MODE:
            // Process set ADCS mode command
            break;
        case CI_CMD_SET_ADCS_TARGET:
            // Process set ADCS target command
            break;
        case CI_CMD_GET_SENSOR_DATA:
            // Process get sensor data command - just temperature sensor and potentiometer for now

            uint8_t response_buf[9];
            response_buf[0] = CI_RESP_SENSOR_DATA;
            response_buf[1] = (uint8_t) ((uint32_t) (current_temperature) >> 24) & 0xFF;
            response_buf[2] = (uint8_t) ((uint32_t) (current_temperature) >> 16) & 0xFF;
            response_buf[3] = (uint8_t) ((uint32_t) (current_temperature) >> 8) & 0xFF;
            response_buf[4] = (uint8_t) ((uint32_t) (current_temperature) & 0xFF);
            response_buf[5] = (uint8_t) ((uint32_t) (current_potentiometer_percentage) >> 24) & 0xFF;
            response_buf[6] = (uint8_t) ((uint32_t) (current_potentiometer_percentage) >> 16) & 0xFF;
            response_buf[7] = (uint8_t) ((uint32_t) (current_potentiometer_percentage) >> 8) & 0xFF;
            response_buf[8] = (uint8_t) ((uint32_t) (current_potentiometer_percentage) & 0xFF);

            CI_sendCommand(response_buf, 9);

            break;
        
            case CI_RESP_SENSOR_DATA:

                uint32_t temp_int = ((uint32_t) buf[1] << 24) | ((uint32_t) buf[2] << 16) | ((uint32_t) buf[3] << 8) | (uint32_t) buf[4];
                float temp_float = (float) temp_int;

                uint32_t pot_int = ((uint32_t) buf[5] << 24) | ((uint32_t) buf[6] << 16) | ((uint32_t) buf[7] << 8) | (uint32_t) buf[8];
                float pot_float = (float) pot_int;

                char prt_buff[100];
                uint8_t prt_len = sprintf(prt_buff, "Temperature: %.2f C \r\n Potentiometer: %.2f%%\r\n", temp_float, pot_float);

                CDC_Transmit_FS(prt_buff, prt_len);
        default:
            return CI_ERR_ERROR;
    }

    return CI_ERR_OK;

}

CI_ERR_e CI_dataReady(void) {

    return CI_ERR_OK;

}