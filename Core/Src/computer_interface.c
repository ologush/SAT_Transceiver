#include "computer_interface.h"
#include "transceiver.h"

#include <stdio.h>
#include "usbd_cdc_if.h"
#include <string.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "commands.h"
#include "helpers.h"

extern float current_temperature;
extern float current_potentiometer_percentage;

extern QueueHandle_t xUSB_txQueue;
extern QueueHandle_t xXCVR_txQueue;

CI_ERR_e CI_processCommand(uint8_t *buf, uint16_t len) {

    CMD_e cmd = buf[0];
    data_packet_s packet;

    switch (cmd) {
        case CMD_SET_ADCS_MODE:

            packet.length = len;
            memcpy(packet.payload, buf, len);
            xQueueSend(xXCVR_txQueue, &packet, portMAX_DELAY);

            break;
        case CMD_SET_ADCS_TARGET:

            packet.length = len;
            memcpy(packet.payload, buf, len);
            xQueueSend(xXCVR_txQueue, &packet, portMAX_DELAY);

            break;
        case CMD_GET_BASE_SENSOR_DATA:

            // Process get sensor data command - just temperature sensor and potentiometer for now
            packet.payload[0] = (uint8_t)CMD_RESP_BASE_SENSOR_DATA;
            floatToBytes(current_temperature, &packet.payload[1]);
            floatToBytes(current_potentiometer_percentage, &packet.payload[5]);

            packet.length = BASE_SENSOR_DATA_LEN;

            xQueueSend(xUSB_txQueue, &packet, portMAX_DELAY);

            break;
        
        case CMD_GET_SAT_TELEMETRY_DATA:

            packet.length = 1;
            packet.payload[0] = (uint8_t)CMD_GET_SAT_TELEMETRY_DATA;
            xQueueSend(xXCVR_txQueue, &packet, portMAX_DELAY);

            break;
        
        default:
            return CI_ERR_ERROR;
    }

    return CI_ERR_OK;

}