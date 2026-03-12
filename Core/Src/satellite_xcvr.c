#include "satellite_xcvr.h"
#include "commands.h"
#include "helpers.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "usart.h"
#include "string.h"

#define ADCS_SENSOR_DATA_RESPONSE_SIZE 12
#define USART_TIMEOUT 100

extern QueueHandle_t xXCVR_txQueue;
extern float current_temperature;
extern float current_potentiometer_percentage;

extern UART_HandleTypeDef huart1;

SAT_XCVR_ERR_e SAT_XCVR_processCommand(data_packet_s *packet) {

    CMD_e cmd = (CMD_e)packet->payload[0];

    switch (cmd) {
        case CMD_SET_ADCS_MODE: {

            uint8_t response;
            // Pass along the set ADCS mode command to the ADCS over USART, should implement an ACK as well
            HAL_UART_Transmit(&huart1, packet->payload, packet->length, USART_TIMEOUT);
            HAL_UART_Receive(&huart1, &response, 1, USART_TIMEOUT); 

            CMD_e responseCmd = (response == CMD_RESP_ACK) ? CMD_RESP_ACK : CMD_RESP_NACK;
            data_packet_s responsePacket;
            responsePacket.payload[0] = (uint8_t)responseCmd;
            responsePacket.payload[1] = cmd;
            responsePacket.length = 2;

            xQueueSend(xXCVR_txQueue, &responsePacket, portMAX_DELAY);

            break;
        }
            
        case CMD_SET_ADCS_TARGET: {

            uint8_t response;
            // Pass along the set ADCS target command to the ADCS over USART, should implement an ACK as well
            HAL_UART_Transmit(&huart1, packet->payload, packet->length, USART_TIMEOUT);
            HAL_UART_Receive(&huart1, &response, 1, USART_TIMEOUT);

            CMD_e responseCmd = (response == CMD_RESP_ACK) ? CMD_RESP_ACK : CMD_RESP_NACK;
            data_packet_s responsePacket;
            responsePacket.payload[0] = (uint8_t)responseCmd;
            responsePacket.payload[1] = cmd;
            responsePacket.length = 2;

            xQueueSend(xXCVR_txQueue, &responsePacket, portMAX_DELAY);

            break;
        }

            
        case CMD_GET_SAT_TELEMETRY_DATA: {
            
            uint8_t usartResponse[ADCS_SENSOR_DATA_RESPONSE_SIZE];

            // Solicit the sensor data from the ADCS over USART
            HAL_StatusTypeDef status;
            status = HAL_UART_Transmit(&huart1, packet->payload, packet->length, USART_TIMEOUT);
            status = HAL_UART_Receive(&huart1, usartResponse, ADCS_SENSOR_DATA_RESPONSE_SIZE, USART_TIMEOUT);

            float current_temp;
            data_packet_s responsePacket;   

            responsePacket.payload[0] = CMD_RESP_SAT_TELEMETRY_DATA;

            // ADF7030 temperature for the response
            ADF7030_getTemperature(&current_temp);

            floatToBytes(current_temp, &responsePacket.payload[1]);
            floatToBytes(current_temperature, &responsePacket.payload[5]);
            floatToBytes(current_potentiometer_percentage, &responsePacket.payload[9]);

            memcpy(&responsePacket.payload[13], usartResponse, ADCS_SENSOR_DATA_RESPONSE_SIZE);

            responsePacket.length = 13 + ADCS_SENSOR_DATA_RESPONSE_SIZE;

            xQueueSend(xXCVR_txQueue, &responsePacket, portMAX_DELAY);

            break;
        }
            

        default:
            return SAT_XCVR_ERR_ERROR;
    }

    return SAT_XCVR_ERR_OK;
}