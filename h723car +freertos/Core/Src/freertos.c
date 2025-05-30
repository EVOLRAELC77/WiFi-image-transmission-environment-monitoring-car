/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "bsp_system.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for camTask */
osThreadId_t camTaskHandle;
const osThreadAttr_t camTask_attributes = {
  .name = "camTask",
  .stack_size = 2048 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for uartTask */
osThreadId_t uartTaskHandle;
const osThreadAttr_t uartTask_attributes = {
  .name = "uartTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for motorTask */
osThreadId_t motorTaskHandle;
const osThreadAttr_t motorTask_attributes = {
  .name = "motorTask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for aht20Task */
osThreadId_t aht20TaskHandle;
const osThreadAttr_t aht20Task_attributes = {
  .name = "aht20Task",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for cam_frame_semaphore */
osSemaphoreId_t cam_frame_semaphoreHandle;
const osSemaphoreAttr_t cam_frame_semaphore_attributes = {
  .name = "cam_frame_semaphore"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void cam_task(void *argument);
void uart_task(void *argument);
void motor_task(void *argument);
void aht20_task(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of cam_frame_semaphore */
  cam_frame_semaphoreHandle = osSemaphoreNew(1, 0, &cam_frame_semaphore_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of camTask */
  camTaskHandle = osThreadNew(cam_task, NULL, &camTask_attributes);

  /* creation of uartTask */
  uartTaskHandle = osThreadNew(uart_task, NULL, &uartTask_attributes);

  /* creation of motorTask */
  motorTaskHandle = osThreadNew(motor_task, NULL, &motorTask_attributes);

  /* creation of aht20Task */
  aht20TaskHandle = osThreadNew(aht20_task, NULL, &aht20Task_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_cam_task */
/**
* @brief Function implementing the camTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_cam_task */
void cam_task(void *argument)
{
  /* USER CODE BEGIN cam_task */
	camera_JPEG_init();
  /* Infinite loop */
  for(;;)
  {
	JPEG_run();
  }
  /* USER CODE END cam_task */
}

/* USER CODE BEGIN Header_uart_task */
/**
* @brief Function implementing the uartTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_uart_task */
void uart_task(void *argument)
{
  /* USER CODE BEGIN uart_task */
  /* Infinite loop */
  for(;;)
  {
	usart_proc();
	osDelay(10);
  }
  /* USER CODE END uart_task */
}

/* USER CODE BEGIN Header_motor_task */
/**
* @brief Function implementing the motorTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_motor_task */
void motor_task(void *argument)
{
  /* USER CODE BEGIN motor_task */
  /* Infinite loop */
  for(;;)
  {
	motor_proc();
	osDelay(10);
  }
  /* USER CODE END motor_task */
}

/* USER CODE BEGIN Header_aht20_task */
/**
* @brief Function implementing the aht20Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_aht20_task */
void aht20_task(void *argument)
{
  /* USER CODE BEGIN aht20_task */
	AHT20_Init();
  /* Infinite loop */
  for(;;)
  {
	AHT20_Read();
    osDelay(300);
  }
  /* USER CODE END aht20_task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

