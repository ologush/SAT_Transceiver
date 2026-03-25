/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f3xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

#define ADCS_COMMAND_STACK_SIZE 128
#define SENSOR_STACK_SIZE 128
#define XCVR_RX_STACK_SIZE 256
#define XCVR_TX_STACK_SIZE 256


#ifdef BASE_STATION
#define USB_TRANSMIT_STACK_SIZE 192
#define USB_RECEIVE_STACK_SIZE 256
#endif

#ifdef SATELLITE
#define SAT_XCVR_COMMAND_STACK_SIZE 512
#endif
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN Private defines */


void vXCVR_TXTask(void * pvParameters);
void vSensorTask(void * pvParameters);
void vXCVR_RXTask(void * pvParameters);
#ifdef BASE_STATION
void vUSBTransmitTask(void * pvParameters);
void vUSBReceiveTask(void * pvParameters);
#endif

#ifdef SATELLITE
void vSAT_XCVR_CommandTask(void * pvParameters);
#endif
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
