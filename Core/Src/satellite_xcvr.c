#include "satellite_xcvr.h"
#include "commands.h"
#include "helpers.h"
#include "FreeRTOS.h"
#include "queue.h"

extern QueueHandle_t xXCVR_txQueue;
extern float current_temperature;
extern float current_potentiometer_percentage;

SAT_XCVR_ERR_e SAT_XCVR_processCommand(data_packet_s *packet) {

    CMD_e cmd = (CMD_e)packet->payload[0];

    switch (cmd) {
        case CMD_SET_ADCS_MODE:
            // Process set ADCS mode command, need UART communication with the ADCS to implement this
            break;
        case CMD_SET_ADCS_TARGET:
            // Process set ADCS target command, need UART communication with the ADCS to implement this
            break;
        case CMD_GET_SAT_SENSOR_DATA:
            // Process get satellite sensor data command, need to implement getting temperature from the ADCS as well

            float current_temp;
            data_packet_s responsePacket;
            responsePacket.payload[0] = CMD_GET_SAT_SENSOR_DATA;

            // ADF7030 temperature for the response
            ADF7030_getTemperature(&current_temp);

            floatToBytes(current_temp, &responsePacket.payload[1]);
            floatToBytes(current_temperature, &responsePacket.payload[5]);
            floatToBytes(current_potentiometer_percentage, &responsePacket.payload[9]);

            responsePacket.length = 13;

            xQueueSend(xXCVR_txQueue, &responsePacket, portMAX_DELAY);

            break;
        default:
            return SAT_XCVR_ERR_ERROR;
    }

    return SAT_XCVR_ERR_OK;
}