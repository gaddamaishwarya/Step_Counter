/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#include <stdio.h>
#include <string.h>
#include <math.h>

void MPU6050_Init(void);
void MPU6050_Read(void);
#define MPU6050_ADDR (0x68 << 1)
uint8_t data[14];
uint16_t g_Ax, g_Ay, g_Az, g_Gx, g_Gy, g_Gz;
char uart_buf[100];

TaskHandle_t sensortaskHandle,pedometertaskhandle,displayhandle;
QueueHandle_t alertQueue;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c3;

UART_HandleTypeDef huart1;

osThreadId defaultTaskHandle;
osThreadId myTask02Handle;
osTimerId myTimer01Handle;
osMutexId myMutex01Handle;
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C3_Init(void);
static void MX_USART1_UART_Init(void);
//void StartDefaultTask(void const * argument);
void StartTask02(void const * argument);
void Callback01(void const * argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void MPU6050_Init(void)
{
  uint8_t check;
  uint8_t data;

  // Check WHO_AM_I
  HAL_I2C_Mem_Read(&hi2c3, MPU6050_ADDR, 0x75, 1, &check, 1, HAL_MAX_DELAY);

  // Wake up MPU6050
  data = 0;
  HAL_I2C_Mem_Write(&hi2c3, MPU6050_ADDR, 0x6B, 1, &data, 1, HAL_MAX_DELAY);

  // Set accel ±2g
  data = 0x00;
  HAL_I2C_Mem_Write(&hi2c3, MPU6050_ADDR, 0x1C, 1, &data, 1, HAL_MAX_DELAY);

  // Set gyro ±250 dps
  data = 0x00;
  HAL_I2C_Mem_Write(&hi2c3, MPU6050_ADDR, 0x1B, 1, &data, 1, HAL_MAX_DELAY);
}
void MPU6050_Read(void)
{
  HAL_I2C_Mem_Read(&hi2c3, MPU6050_ADDR, 0x3B, 1, data, 14, HAL_MAX_DELAY);

  g_Ax = (int16_t)(data[0] << 8 | data[1]);
  g_Ay = (int16_t)(data[2] << 8 | data[3]);
  g_Az = (int16_t)(data[4] << 8 | data[5]);

  g_Gx = (int16_t)(data[8] << 8 | data[9]);
  g_Gy = (int16_t)(data[10] << 8 | data[11]);
  g_Gz = (int16_t)(data[12] << 8 | data[13]);
}

void SensorTask(void*arg);
void Pedometertask(void*arg);
void Displaytask(void*arg);
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C3_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  MPU6050_Init(); //imp

  // queue to send the data form pedometer to the alert task
  alertQueue = xQueueCreate(5,sizeof(int));

  TaskHandle_t sensortaskHandle,pedometertaskhandle,displayhandle;

  xTaskCreate(SensorTask,"Sensor",128, NULL, 1,&sensortaskHandle);
  xTaskCreate(Pedometertask,"Pedometer",128, NULL, 2,&pedometertaskhandle);
  xTaskCreate(Displaytask,"Display",256, NULL,1,&displayhandle);


  vTaskStartScheduler(); // oskernal start

  /* USER CODE END 2 */

  /* Create the mutex(es) */
  /* definition and creation of myMutex01 */
//  osMutexDef(myMutex01);
//  myMutex01Handle = osMutexCreate(osMutex(myMutex01));

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* Create the timer(s) */
  /* definition and creation of myTimer01 */
//  osTimerDef(myTimer01, Callback01);
//  myTimer01Handle = osTimerCreate(osTimer(myTimer01), osTimerPeriodic, NULL);

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
//  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
//  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of myTask02 */
//  osThreadDef(myTask02, StartTask02, osPriorityIdle, 0, 128);
//  myTask02Handle = osThreadCreate(osThread(myTask02), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Start scheduler */
  //osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C3_Init(void)
{

  /* USER CODE BEGIN I2C3_Init 0 */

  /* USER CODE END I2C3_Init 0 */

  /* USER CODE BEGIN I2C3_Init 1 */

  /* USER CODE END I2C3_Init 1 */
  hi2c3.Instance = I2C3;
  hi2c3.Init.ClockSpeed = 100000;
  hi2c3.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c3.Init.OwnAddress1 = 0;
  hi2c3.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c3.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c3.Init.OwnAddress2 = 0;
  hi2c3.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c3.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c3) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c3, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c3, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C3_Init 2 */

  /* USER CODE END I2C3_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13|GPIO_PIN_14, GPIO_PIN_RESET);

  /*Configure GPIO pins : PG13 PG14 */
  GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

//PedometerTask
//      │
//      │  xQueueSend(alertQueue, &step_count)
//      ▼
//Queue
//      ▼
//DisplayTask
//      │
//      │ xQueueReceive()
//      ▼
//Print steps to UART

void SensorTask(void *arg)
{
	for(;;)
	{
		MPU6050_Read();  // read the sensor

//		sprintf(uart_buf, "Ax=%d Ay=%d Az=%d | Gx=%d Gy=%d Gz=%d\r\n", g_Ax, g_Ay, g_Az, g_Gx, g_Gy, g_Gz);
//
//		HAL_UART_Transmit(&huart1, (uint8_t*)uart_buf, strlen(uart_buf), portMAX_DELAY);

		HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_13);  // sending the sensor task

		vTaskDelay(pdMS_TO_TICKS(20));
	}
}
void Pedometertask(void*arg)
{
	int step_count = 0;
	float prev_magnitude = 0.0f;
	float magnitude;
	while(1)
	{
		// TODO: Calculate acceleration magnitude
		magnitude = sqrt(g_Ax*g_Ax +g_Ay*g_Ay +g_Az*g_Az);

		// TODO: Calculate swing speed from gyroscope
		float gyro_dps = g_Gy / 131.0;

		// TODO: Implement the step detection logic (Threshold + Edge + Swing)
		if(magnitude > 18000 && prev_magnitude < 18000 &&fabs(gyro_dps) > 15)
		{
			step_count++;

			// TODO: If step detected, send to queue and apply cooldown delay

			xQueueSend(alertQueue, &step_count, 0);

			vTaskDelay(pdMS_TO_TICKS(250));
		}

		// TODO: Update prev_magnitude
		prev_magnitude = magnitude;
		vTaskDelay(pdMS_TO_TICKS(20));
	}
}

void Displaytask(void*arg)
{
	int steps;
	uint16_t uart_buf[50];
	for(;;)
	{
			xQueueReceive(alertQueue,&steps,portMAX_DELAY);

			sprintf(uart_buf,"Steps: %d\r\n",steps);

			HAL_UART_Transmit(&huart1,(uint8_t*)uart_buf,strlen(uart_buf),HAL_MAX_DELAY);
	}
}
/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN 5 */
//  /* Infinite loop */
//  for(;;)
//  {
//    osDelay(1);
//  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_StartTask02 */
/**
* @brief Function implementing the myTask02 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask02 */
void StartTask02(void const * argument)
{
  /* USER CODE BEGIN StartTask02 */
//  /* Infinite loop */
//  for(;;)
//  {
//    osDelay(1);
//  }
  /* USER CODE END StartTask02 */
}

/* Callback01 function */
void Callback01(void const * argument)
{
  /* USER CODE BEGIN Callback01 */
//
  /* USER CODE END Callback01 */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */
//
  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */
//
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
#ifdef USE_FULL_ASSERT
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
