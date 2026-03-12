/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "adc.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "task.h"
#include "transceiver.h"
#include "temp_sensor.h"
#include "potentiometer.h"
#include "usbd_cdc_if.h"
#include "__public__ADF7030_1_fw_macro.h"
#include "semphr.h"
#include "commands.h"

#ifdef BASE_STATION
#include "computer_interface.h"
#endif

#ifdef SATELLITE
#include "satellite_xcvr.h"
#endif
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#ifdef BASE_STATION
#define USB_RX_BUF_SIZE 128
#endif

#define ADC_NUM_CONVERSIONS 2

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

static uint16_t adc_data[ADC_NUM_CONVERSIONS];

float current_temperature;
float current_potentiometer_percentage;


#ifdef BASE_STATION
QueueHandle_t xUSB_txQueue;
QueueHandle_t xUSB_rxQueue;

SemaphoreHandle_t xUSBMutex;
SemaphoreHandle_t xUSBReceiveSemaphore;

extern uint8_t UserRxBufferFS[];
extern USBD_HandleTypeDef hUsbDeviceFS;
uint32_t usbRxLen;
#endif

#ifdef SATELLITE
QueueHandle_t xUART_txQueue;
QueueHandle_t xUART_rxQueue;

#endif

QueueHandle_t xXCVR_txQueue;
QueueHandle_t xXCVR_rxQueue;

static SemaphoreHandle_t xXCVRMutex;
static SemaphoreHandle_t xRXReadySemaphore;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
  BaseType_t xTransmitTaskReturned;
  BaseType_t xXCVR_RXTaskReturned;
  BaseType_t xSensorTaskReturned;


  TaskHandle_t xTransmitHandle = NULL;
  TaskHandle_t xReceiveHandle = NULL;
  TaskHandle_t xSensorHandle = NULL;

#ifdef BASE_STATION
  xUSB_txQueue = xQueueCreate(5, sizeof(data_packet_s));
  xUSB_rxQueue = xQueueCreate(5, sizeof(data_packet_s));

  xUSBMutex = xSemaphoreCreateMutex();
  xUSBReceiveSemaphore = xSemaphoreCreateBinary();

  TaskHandle_t xUSBTransmitHandle = NULL;
  TaskHandle_t xUSBReceiveHandle = NULL;

  BaseType_t xUSBTransmitTaskReturned;
  BaseType_t xUSBReceiveTaskReturned;
#endif

#ifdef SATELLITE
  BaseType_t xSAT_XCVR_CommandTaskReturned;

  TaskHandle_t xSAT_XCVR_CommandHandle = NULL;
#endif

  xXCVRMutex = xSemaphoreCreateMutex();
  xRXReadySemaphore = xSemaphoreCreateBinary();
  

  xXCVR_txQueue = xQueueCreate(5, sizeof(data_packet_s));
  xXCVR_rxQueue = xQueueCreate(5, sizeof(data_packet_s));
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  xTransmitTaskReturned = xTaskCreate(
                            vXCVR_TXTask,
                            "Transmit",
                            XCVR_TX_STACK_SIZE,
                            NULL,
                            2,
                            &xTransmitHandle);

  
  xXCVR_RXTaskReturned = xTaskCreate(
                          vXCVR_RXTask,
                          "XCVR_RX",
                          XCVR_RX_STACK_SIZE,
                          NULL,
                          1,
                          &xReceiveHandle);

  
#ifdef BASE_STATION

  xUSBTransmitTaskReturned = xTaskCreate(
                              vUSBTransmitTask,
                              "USBTransmit",
                              USB_TRANSMIT_STACK_SIZE,
                              NULL,
                              2,
                              &xUSBTransmitHandle);

  xUSBReceiveTaskReturned = xTaskCreate(
                              vUSBReceiveTask,
                              "USBReceive",
                              USB_RECEIVE_STACK_SIZE,
                              NULL,
                              2,
                              &xUSBReceiveHandle);


#endif
#ifdef SATELLITE
  
  xSAT_XCVR_CommandTaskReturned = xTaskCreate(
                              vSAT_XCVR_CommandTask,
                              "SAT_XCVR_Command",
                              SAT_XCVR_COMMAND_STACK_SIZE,
                              NULL,
                              3,
                              &xSAT_XCVR_CommandHandle);

#endif
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_USART1_Init();
  MX_SPI1_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  MX_USB_DEVICE_Init();

  // Turn on XCVR power
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_SET);

  ADF7030_init(&hspi1, GPIOB, GPIO_PIN_0, GPIOC, GPIO_PIN_15, GPIOA, GPIO_PIN_6);

  HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
  HAL_ADC_Start_DMA(&hadc1, (uint32_t *) adc_data, ADC_NUM_CONVERSIONS);

  HAL_TIM_Base_Start(&htim3);
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* Call init function for freertos objects (in cmsis_os2.c) */
  MX_FREERTOS_Init();

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  vTaskStartScheduler();
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB|RCC_PERIPHCLK_USART1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.USBClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enables the Clock Security System
  */
  HAL_RCC_EnableCSS();
}

/* USER CODE BEGIN 4 */
void vXCVR_TXTask(void *pvParameters) {
  data_packet_s packetToSend;
  for(;;) {

    // Pull a packet off the transmit queue and send it via the transceiver
    // Should disable the input interrupt during this as the ISR will get triggered for receiving
    xQueueReceive(xXCVR_txQueue, &packetToSend, portMAX_DELAY);
    xSemaphoreTake(xXCVRMutex, portMAX_DELAY);

    // Disable the interrupt tied to the XCVR as this gets flagged when a transmission or a reception is complete
    HAL_NVIC_DisableIRQ(EXTI1_IRQn);

    ADF7030_transmitPacket(&packetToSend);

    // Clear the interrupt flag and re-enable the interrupt
    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_1);
    HAL_NVIC_EnableIRQ(EXTI1_IRQn);

    xSemaphoreGive(xXCVRMutex);
  }

  vTaskDelete(NULL);
}

void vXCVR_RXTask(void * pvParameters) {

  data_packet_s receivedPacket;
  for(;;) {

    // Wait until a packet has been received, then read it
    xSemaphoreTake(xRXReadySemaphore, portMAX_DELAY);
    xSemaphoreTake(xXCVRMutex, portMAX_DELAY);

    TRANSCEIVER_ERR_e result = ADF7030_receivePacket(&receivedPacket);
    xSemaphoreGive(xXCVRMutex);

    if (result != TRANSCEIVER_ERR_OK) {
      continue;
    }

    // Depending on whether this is a satellite or a base station, push the received packet to the appropriate queue for processing
#ifdef BASE_STATION
    xQueueSend(xUSB_txQueue, &receivedPacket, portMAX_DELAY);
#endif
#ifdef SATELLITE
    xQueueSend(xXCVR_rxQueue, &receivedPacket, portMAX_DELAY);
#endif
  }

  vTaskDelete(NULL);
}

#ifdef BASE_STATION
void vUSBReceiveTask(void *pvParameters) {

  uint8_t cmdBuf[USB_RX_BUF_SIZE];
  uint32_t cmdLen;

  for(;;) {

    // Wait until a packet has been received over USB, then push it to the USB receive queue
    xSemaphoreTake(xUSBReceiveSemaphore, portMAX_DELAY);

    // Copy usb data to a local buffer
    cmdLen = usbRxLen;
    memcpy(cmdBuf, UserRxBufferFS, cmdLen);

    // Re-arm USB
    USBD_CDC_SetRxBuffer(&hUsbDeviceFS, &UserRxBufferFS[0]);
    USBD_CDC_ReceivePacket(&hUsbDeviceFS);

    // Process the command received over USB
    CI_processCommand(cmdBuf, cmdLen);
  }

  vTaskDelete(NULL);
}

void vUSBTransmitTask(void *pvParameters) {

  for(;;) {

    // Wait until there is a packet to send over USB, then send it
    data_packet_s packetToSend;
    xQueueReceive(xUSB_txQueue, &packetToSend, portMAX_DELAY);

    // Wait until the USB is ready to transmit, then send the packet over USB
    xSemaphoreTake(xUSBMutex, portMAX_DELAY);
    USBD_CDC_SetTxBuffer(&hUsbDeviceFS, packetToSend.payload, packetToSend.length);
    USBD_CDC_TransmitPacket(&hUsbDeviceFS);
    xSemaphoreGive(xUSBMutex);
  }

  vTaskDelete(NULL);
}
#endif
#ifdef SATELLITE

void vSAT_XCVR_CommandTask(void * pvParameters) {

  for(;;) {

    // Wait until there is a packet received from the transceiver, then process it
    data_packet_s receivedPacket;
    xQueueReceive(xXCVR_rxQueue, &receivedPacket, portMAX_DELAY);

    SAT_XCVR_processCommand(&receivedPacket);
  }

  vTaskDelete(NULL);
}

#endif

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
  if(GPIO_Pin == GPIO_PIN_1) {

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(xRXReadySemaphore, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
  if (hadc->Instance == ADC1) {
    current_potentiometer_percentage = potentiometer_ADCToPercentage(adc_data[0]);
    current_temperature = temp_sensor_ADCToTemperature(adc_data[1]);
  }
}



/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
