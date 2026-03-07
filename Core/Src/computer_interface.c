#include "computer_interface.h"
#include "transceiver.h"

#include <stdio.h>
#include "usbd_cdc_if.h"
#include <string.h>
#include "FreeRTOS.h"
#include "queue.h"


extern float current_temperature;
extern float current_potentiometer_percentage;


extern QueueHandle_t xUSB_txQueue;
extern QueueHandle_t xXCVR_txQueue;

CI_ERR_e CI_processCommand(uint8_t *buf, uint16_t len) {

    CI_CMD_e cmd = buf[0];
    data_packet_s packet;

    switch (cmd) {
        case CI_CMD_SET_ADCS_MODE:

            packet.length = len;
            memcpy(packet.payload, buf, len);
            xQueueSend(xXCVR_txQueue, &packet, portMAX_DELAY);

            break;
        case CI_CMD_SET_ADCS_TARGET:

            packet.length = len;
            memcpy(packet.payload, buf, len);
            xQueueSend(xXCVR_txQueue, &packet, portMAX_DELAY);

            break;
        case CI_CMD_GET_BASE_SENSOR_DATA:
            // Process get sensor data command - just temperature sensor and potentiometer for now

            packet.payload[0] = (uint8_t)CI_RESP_SENSOR_DATA;
            packet.payload[1] = (uint8_t) ((uint32_t) (current_temperature) >> 24) & 0xFF;
            packet.payload[2] = (uint8_t) ((uint32_t) (current_temperature) >> 16) & 0xFF;
            packet.payload[3] = (uint8_t) ((uint32_t) (current_temperature) >> 8) & 0xFF;
            packet.payload[4] = (uint8_t) ((uint32_t) (current_temperature) & 0xFF);
            packet.payload[5] = (uint8_t) ((uint32_t) (current_potentiometer_percentage) >> 24) & 0xFF;
            packet.payload[6] = (uint8_t) ((uint32_t) (current_potentiometer_percentage) >> 16) & 0xFF;
            packet.payload[7] = (uint8_t) ((uint32_t) (current_potentiometer_percentage) >> 8) & 0xFF;
            packet.payload[8] = (uint8_t) ((uint32_t) (current_potentiometer_percentage) & 0xFF);

            xQueueSend(xUSB_txQueue, &packet, portMAX_DELAY);

            break;
        
        default:
            return CI_ERR_ERROR;
    }

    return CI_ERR_OK;

}