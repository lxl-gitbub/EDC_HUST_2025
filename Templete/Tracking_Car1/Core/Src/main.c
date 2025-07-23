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
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "Mem.h"
#include "Hall_Encoder.h"
#include "tb6612fng.h"

#include "xunxian_delay.h"
#include "app_usart2.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

//These are the value of the changing value between two sampling time point,
//meaningt that they are not the speed the value in real world.

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
Mem l, r;
char message[100];
extern u8 g_new_package_flag;
uint8_t Rx2_Temp; // ���ڽ���USART2���ݵ���ʱ����
int straight_flag = 0; //�����ж��Ƿ��Ѿ���⵽Ŀ��
Motor Left, Right;//Define the structures that control left motor and right motor
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Initialize(void);
static void MPU_Config(void);
/* USER CODE BEGIN PFP */
void lineWalking(void);
void enable_Buzzer(void);
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

  /* USER CODE END 1 */

  /* Enable I-Cache---------------------------------------------------------*/
  SCB_EnableICache();

  /* Enable D-Cache---------------------------------------------------------*/
  SCB_EnableDCache();

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
	
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM7_Init();
  MX_USART1_UART_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM4_Init();
  MX_TIM3_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  MX_TIM13_Init();
  /* USER CODE BEGIN 2 */
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_RESET); 
	HAL_TIM_Base_Stop_IT(&htim13); // ֹͣTIM3��ʱ���ж�
	__HAL_TIM_SET_COUNTER(&htim13, 0);
	EncoderInit(&htim2, TIM_CHANNEL_1, TIM_CHANNEL_2, &htim7);
	EncoderInit(&htim1, TIM_CHANNEL_1, TIM_CHANNEL_2, &htim7);
	MotorInit(&Right, GPIOD, GPIO_PIN_3, GPIOD, GPIO_PIN_4,  &htim4, TIM_CHANNEL_1, 0);
	MotorInit(&Left, GPIOD, GPIO_PIN_5, GPIOD, GPIO_PIN_6,  &htim3, TIM_CHANNEL_3, 0);
	//Please goto tb6612fng.c and tb6612fng.h to see the usage and defination of motor functions and structure
	//While using this function, 
	//please connect PD3 to AIN2, PD4 to AIN1, PD5 to BIN2, PD6 to BIN1,
	//PD12 to PWMA and PB0 to PWMB to make deriction right.
	MemInit(&l, 100, 1, &huart1);

	//�ȴ������ȶ�
	HAL_Delay(2000);
	//��·Ѳ������ʼ����
	HAL_UART_Receive_IT(&huart3, &Rx2_Temp, 1);
	HAL_UART_Transmit(&huart3, (uint8_t *)"$0,0,1#", 7, HAL_MAX_DELAY);
	HAL_UART_Receive_IT(&huart3, &Rx2_Temp, 1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		/*MotorSet(FOR, 999, &Left);
		MotorSet(SLIDE, 999, &Right);
		MotorSet(BACK, 500, &Right);
		MotorSet(BREAK, 999, &Right); */

		if(g_new_package_flag == 1)
		{
			g_new_package_flag = 0;
			Deal_Usart_Data();
			//sprintf(message,"x1:%d,x2:%d,x3:%d,x4:%d,x5:%d,x6:%d,x7:%d,x8:%d\r\n",IR_Data_number[0],IR_Data_number[1],IR_Data_number[2],IR_Data_number[3],IR_Data_number[4],IR_Data_number[5],IR_Data_number[6],IR_Data_number[7]);
			//HAL_UART_Transmit(&huart1,(uint8_t *)message, strlen(message), HAL_MAX_DELAY);  
			lineWalking();
		}
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
	MemOut(&l);
	destoryMem(&l);
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

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
// ...existing code...
double gr(void)
{
	return getSpeed(0);
}

void enable_Buzzer(void){
	HAL_TIM_Base_Stop_IT(&htim13); // ֹͣTIM3��ʱ���ж�
	 HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_SET); 
	__HAL_TIM_SET_COUNTER(&htim13, 0);
	 HAL_TIM_Base_Start_IT(&htim13); 
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	UpdateAllSpeed(htim);
	MemSet(&l, gr);
	
	if (htim->Instance == TIM13){
		 HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_RESET); 
		HAL_TIM_Base_Stop_IT(&htim13); // ֹͣTIM3��ʱ���ж�
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    // �ж��ж��Ƿ�����USART3
    if (huart->Instance == USART3)
    {
        // ��ʱ�����յ���һ���ֽ������Ѿ���HAL���Զ�������Rx2_Temp������
        // �����������ݴ�����
        Deal_IR_Usart(Rx2_Temp);
        
        // ���ؼ��������ٴε���HAL_UART_Receive_IT()��������׼����һ�ε����ݽ���
        HAL_UART_Receive_IT(&huart3, &Rx2_Temp, 1);
    }
}
// ...existing code...
/* USER CODE END 4 */

/* MPU Configuration */

void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x24000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_512KB;
  MPU_InitStruct.SubRegionDisable = 0x0;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL1;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Number = MPU_REGION_NUMBER1;
  MPU_InitStruct.BaseAddress = 0x30000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_128KB;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Number = MPU_REGION_NUMBER2;
  MPU_InitStruct.BaseAddress = 0x30020000;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Number = MPU_REGION_NUMBER3;
  MPU_InitStruct.BaseAddress = 0x30040000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_32KB;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Number = MPU_REGION_NUMBER4;
  MPU_InitStruct.BaseAddress = 0x38000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_64KB;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

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
