#include "satellite_xcvr.h"
#include "commands.h"
#include "helpers.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "usart.h"
#include "string.h"
#include "semphr.h"


#define UART_TIMEOUT 10000

extern QueueHandle_t xXCVR_txQueue;
extern float current_temperature;
extern float current_potentiometer_percentage;
extern SemaphoreHandle_t xUARTRxSemaphore;

extern UART_HandleTypeDef huart1;

SAT_XCVR_ERR_e SAT_XCVR_processCommand(data_packet_s *packet) {

    CMD_e cmd = (CMD_e)packet->payload[0];

    switch (cmd) {
        case CMD_SET_ADCS_MODE: {

            uint8_t uartResponse;
            // Pass along the set ADCS mode command to the ADCS over UART, should implement an ACK as well
            HAL_UART_Receive_DMA(&huart1, &uartResponse, 1);
            HAL_UART_Transmit(&huart1, packet->payload, packet->length, UART_TIMEOUT);
            if (xSemaphoreTake(xUARTRxSemaphore, pdMS_TO_TICKS(UART_TIMEOUT)) != pdTRUE) {
                HAL_UART_DMAStop(&huart1);
                memset(&uartResponse, 0, 1);
            }

            CMD_e responseCmd = (uartResponse == CMD_RESP_ACK) ? CMD_RESP_ACK : CMD_RESP_NACK;
            data_packet_s responsePacket;
            responsePacket.payload[0] = (uint8_t)responseCmd;
            responsePacket.payload[1] = cmd;
            responsePacket.length = 2;

            xQueueSend(xXCVR_txQueue, &responsePacket, portMAX_DELAY);

            break;
        }
            
        case CMD_SET_ADCS_TARGET: {

            uint8_t uartResponse;
            // Pass along the set ADCS target command to the ADCS over UART, should implement an ACK as well
            HAL_UART_Receive_DMA(&huart1, &uartResponse, 1);
            HAL_UART_Transmit(&huart1, packet->payload, packet->length, UART_TIMEOUT);
            if (xSemaphoreTake(xUARTRxSemaphore, pdMS_TO_TICKS(UART_TIMEOUT)) != pdTRUE) {
                HAL_UART_DMAStop(&huart1);
                memset(&uartResponse, 0, 1);
            }

            CMD_e responseCmd = (uartResponse == CMD_RESP_ACK) ? CMD_RESP_ACK : CMD_RESP_NACK;
            data_packet_s responsePacket;
            responsePacket.payload[0] = (uint8_t)responseCmd;
            responsePacket.payload[1] = cmd;
            responsePacket.length = 2;

            xQueueSend(xXCVR_txQueue, &responsePacket, portMAX_DELAY);

            break;
        }

            
        case CMD_GET_SAT_TELEMETRY_DATA: {
            
            uint8_t uartResponse[ADCS_SENSOR_DATA_RESPONSE_SIZE];

            // Solicit the sensor data from the ADCS over UART
            HAL_StatusTypeDef status;

            // Only send the command byte - packet->length includes RF padding zeros
            uint8_t adcs_cmd = packet->payload[0];
            HAL_UART_Receive_DMA(&huart1, uartResponse, ADCS_SENSOR_DATA_RESPONSE_SIZE);
            status = HAL_UART_Transmit(&huart1, &adcs_cmd, 1, UART_TIMEOUT);

            if (xSemaphoreTake(xUARTRxSemaphore, pdMS_TO_TICKS(UART_TIMEOUT)) != pdTRUE) {
                HAL_UART_DMAStop(&huart1);
                memset(uartResponse, 0, ADCS_SENSOR_DATA_RESPONSE_SIZE);
            }
            float current_xcvr_temp;
            data_packet_s responsePacket;   

            responsePacket.payload[0] = CMD_RESP_SAT_TELEMETRY_DATA;

            // ADF7030 temperature for the response
            //ADF7030_getTemperature(&current_xcvr_temp);

            //floatToBytes(current_xcvr_temp, &responsePacket.payload[1]);
            floatToBytes(current_temperature, &responsePacket.payload[1]);
            floatToBytes(current_potentiometer_percentage, &responsePacket.payload[5]);

            memcpy(&responsePacket.payload[9], uartResponse, ADCS_SENSOR_DATA_RESPONSE_SIZE);
            float yaw = bytesToFloat(uartResponse);
            float yawRate = bytesToFloat(&uartResponse[4]);
            float IMUtemp = bytesToFloat(&uartResponse[8]);
            responsePacket.length = 9 + ADCS_SENSOR_DATA_RESPONSE_SIZE;

            uint8_t testData[21] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF, 0x10, 0x11, 0x12, 0x13, 0x14};
            uint8_t testData2[21] = {0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0xAB};
            memcpy(responsePacket.payload, testData2, 21);

            xQueueSend(xXCVR_txQueue, &responsePacket, portMAX_DELAY);

            break;
        }
            

        default:
            return SAT_XCVR_ERR_ERROR;
    }

    return SAT_XCVR_ERR_OK;
}