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
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include <string.h>
#include "core_cm4.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
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

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


// ------串口通信相关参数初始化----------
// 串口接线:(按照我的f4板子和CanMV)
// STM32_TX == PA9  ----- 12 == CanMV_RX(上)
// STM32_RX == PA10 ----- 11 == CanMV_TX(下)

// 定义接受长度
#define USART_RX_BUF_LEN 25 
// 接收状态和缓冲区(用于该串口通信的各种信息的提取)
uint8_t USART_RX_BUF[USART_RX_BUF_LEN] ;
// 激光坐标记录数组
uint8_t Laser_Loc[USART_RX_BUF_LEN] ;

// ------串口-激光通信相关参数初始化------
// 目标激光(紫光)的(x,y)
int target_x ;
int target_y ;
// 当前执行追踪任务的激光的(6x,y)
int real_x ;
int real_y ;

// 标志位处理逻辑,由于该程序目前作为部分功能测试,flag直接置1
int flag = 1 ; // flag = 0 : 空闲	 flag = 1 : 激光追踪模式

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
  MX_TIM3_Init();
  MX_TIM5_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

	// -------------主程序(setup)-------------
	
  // 接收中断
  HAL_UARTEx_ReceiveToIdle_IT(&huart1, USART_RX_BUF, USART_RX_BUF_LEN);
  // 滴答定时器启动
  HAL_SYSTICK_Config(SystemCoreClock/1000) ;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  

  // -------------主程序(loop)-------------
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */
}
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	if (huart->Instance == USART1)
	{
 		// ------------激光模式判定------------
		if ( flag == 1 )
		{
			// 发布激光坐标数据请求
		  if (huart1.gState == HAL_UART_STATE_READY) 
			{ 
				HAL_UART_Transmit_IT(&huart1, (uint8_t *)"L", 1); // 发送请求
			}
				
			// 激光位置返回帧: 0x12 target_x target_y real_x real_y 0x5B
			if ( USART_RX_BUF[0] == 0x12 && USART_RX_BUF[5] == 0x5B )
			{
				// LED电平翻转,指示成功读取相关数据
				HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin);
				
				
				// 得到激光相关数据
				for ( int j = 0 ; j < 4; j ++ )
				{
					Laser_Loc[j] = (int)USART_RX_BUF[j + 1] * 4 ; // 得到真实坐标
				}
			}
			else
			{
				// 报错机制,待开发
			}
		}
		
		// ------------其他模式判定------------
		// 待开发
		
		// ------------接收信息通用处理---------

		// 重启接收前清除缓存区
		memset(USART_RX_BUF, 0, USART_RX_BUF_LEN);
 
	  // 再次接收
		HAL_UARTEx_ReceiveToIdle_IT(&huart1, USART_RX_BUF, USART_RX_BUF_LEN);
	}
}

// 暂时不用
void HAL_SYSTICK_Callback(void)
{
//	static uint32_t count = 0 ;
//	count ++ ;

//	if (count >= 1000) // 1s
//	{
//		count = 0 ;
//		const char *Send_MCU = "Check"; // 带换行的回显字符串
//		uint16_t Send_MCU_len = strlen(Send_MCU); // 计算长度	
//		
//		
//		if (huart1.gState == HAL_UART_STATE_READY) 
//		{ 
//		  HAL_UART_Transmit_IT(&huart1, (uint8_t *)Send_MCU, Send_MCU_len); // 检查UART是否就绪
//		}
//	}
}

/* USER CODE END 4 */

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
