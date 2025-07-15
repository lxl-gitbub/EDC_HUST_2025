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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "AllHeader.h"
#include "mode.h"
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
// MS601M ���������ݽṹ��
atk_ms601m_attitude_data_t attitude_dat;
atk_ms601m_gyro_data_t gyro_dat;
atk_ms601m_accelerometer_data_t accelerometer_dat;
char message[256]; 

//��Ϊ���������ݱ���
int Digtal[8];

MODE mode = {WAIT_MODE, ZERO}; // ��ʼ��ģʽΪ��ҩģʽ��λ��Ϊ��
short drug_change = 1;
//���ڱ���Ƿ���Ҫ����ҩ��ģʽ��ת����
//ֻ�����ʼ��ʱ����1���Լ���������ֹͣ��ʱ����1���м�Ϊ0�����м䲻��Ҫ����Ƿ�װҩ
uint32_t mode_begin_t = 0;//��¼ģʽ��ʼ��ʱ��

//��������ת�����
const float r = 0.17;
const float tel = 26;

float current_yaw;//���ڴ洢���ڵ�yaw

const int back_delay = 300;//��ֹ��ͷ���ں�ת֮��ֹͣһ��ʱ��
const float back_angle_cor = 5;//���ھ��������ǵ�ϵͳ�����Ƕȹ�С����ʱ�벻���ܣ�
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Initialize(void);
static void MPU_Config(void);
/* USER CODE BEGIN PFP */
bool CheckAndTurn(void);
bool CheckAndEnd(void);
void RED_up(void);
void RED_down(void);
void GREEN_up(void);
void GREEN_down(void);
void YELLOW_up(void);
void YELLOW_down(void);
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
  MX_TIM3_Init();
  MX_TIM5_Init();
  MX_TIM6_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
	//�����ǳ�ʼ��
	atk_ms601m_init(115200);

	// ����ͱ�������ʼ��	
  Motor Left, Right;  
	MEInit(&Left, &Right); 
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    IIC_Get_Digtal(Digtal); // ��ȡ���ִ���������,ÿһ����Ҫִ���Ի�ȡ����
		lineWalking();
		if(Road_detect(0, 0))
		{
			Break();
			break;
		}

//    if(drug_change)
//    {
//      if(drugSet(&mode))// ����ҩ��ģʽ��ת�������ת���ɹ�����if
//      {
//        drug_change = 0; // ת����ɺ󽫱�־λ��Ϊ0
//        mode_begin_t = HAL_GetTick(); // ��¼ģʽ��ʼ��ʱ��
//      }
//      continue; // ������һ��ѭ��
//    }
//    switch (mode.drug)
//    {
//      case WAIT_MODE:
//        // �ȴ�ģʽ�µĴ����߼�
//        break;
//      case PROPEL_MODE:
//        // ҩ��ģʽ�µĴ����߼�
//        switch (mode.loc)
//        {
//          case ZERO:
//          case ONE:
//          case TWO:
//          case L3:
//          case R3:
//            // ����Щλ���µĴ����߼���
//						if(!CheckAndTurn())
//              lineWalking(); // ����ѭ������
//            break;
//          case L1:
//          case L2:
//          case R1:
//          case R2:
//          case L3_L:
//          case L3_R:
//          case R3_L:
//          case R3_R:
//            // ����Щλ���µĴ����߼�
//            if(!CheckAndEnd()) // ����Ƿ���Ҫ������ǰģʽ
//              lineWalking(); // ����ѭ������
//            break;
//					}
//        break;
//      case RETURN_MODE:
//        // ����ģʽ�µĴ����߼�
//        switch(mode.loc)
//        {
//          case L1:
//          case L2:
//            // ����Щλ���µĴ����߼�
//            if(!CheckAndTurn())
//              Back(90 + back_angle_cor); // ����,��90��Ϊ����
//						else
//							HAL_Delay(back_delay);
//            break; // ���������ӷ���ģʽ�µľ����߼�
//          case R1:
//          case R2:
//            // ����Щλ���µĴ����߼�
//            if(!CheckAndTurn())
//              Back(-90 + back_angle_cor); // ����,��-90��Ϊ����
//						else
//							HAL_Delay(back_delay);
//            break; // ���������ӷ���ģʽ�µľ����߼�
//          case L3_L:
//          case R3_R:
//            // ����Щλ���µĴ����߼�
//            if(!CheckAndTurn()) // ����Ƿ���Ҫ������ǰģʽ
//              Back(180 + back_angle_cor); // ����,��180��Ϊ����
//						else
//							HAL_Delay(back_delay);
//            break; // ���������ӷ���ģʽ�µľ����߼�
//          case L3_R:
//          case R3_L:
//            // ����Щλ���µĴ����߼�
//            if(!CheckAndTurn()) // ����Ƿ���Ҫ������ǰģʽ
//              Back(0 + back_angle_cor); // ����,��0��Ϊ����
//						else
//							HAL_Delay(back_delay);
//            break; // ���������ӷ���ģʽ�µľ����߼�
//          case ONE:
//          case TWO:
//          case L3:
//          case R3:
//            // ����Щλ���µĴ����߼�
//            if(!CheckAndTurn()) // ����Ƿ���Ҫ������ǰģʽ
//              lineWalking(); // ����ѭ������
//            break; // ���������ӷ���ģʽ�µľ����߼�
//          case ZERO:
//            // ����λ���µĴ����߼�
//            if(!CheckAndEnd()) // ����Ƿ���Ҫ������ǰģʽ
//              lineWalking(); // ����ѭ������
//            break; // ���������ӷ���ģʽ�µľ����߼�
//        }
//        break;
//      default:
//        // δ֪ģʽ�Ĵ����߼�
//        break;
//    }
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
//���ڸ��������ṩ1us�Ķ�ʱ
void delay_us_hal(uint16_t nus)
{
    __HAL_TIM_SET_COUNTER(&htim6, 0);
    HAL_TIM_Base_Start(&htim6);       
    while (__HAL_TIM_GET_COUNTER(&htim6) < nus); 
    HAL_TIM_Base_Stop(&htim6);        
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
}

// ��ʱ���жϻص����������ڸ��±������ٶ�
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    // �������б��������ٶ�����
    UpdateAllSpeed(htim);
}

bool CheckAndTurn()
{
  if(HAL_GetTick() - mode_begin_t > 1000)
  {
    if(cross_Roads_Detect())
    {
      switch(DirGet(&mode)) // ��ȡ��һ������
      {
				case BACKWARD://�ڵڶ�����õ���Ŀǰ�����õ�
        case FORWARD:
          return true; // ����ǰ��
        case LEFT:
					current_yaw = getYaw();
          while(!isInTheYaw(sumTheta(current_yaw, 90), tel)) {runCircle(r, 0.5, 90, LEFT); HAL_Delay(10);}
          Break();
          return true; // ����ǰ��
        case RIGHT:
					current_yaw = getYaw();
          while(!isInTheYaw(sumTheta(current_yaw, -90), tel)){runCircle(r, 0.5, 90, RIGHT); HAL_Delay(10);}
          Break();
          return true; // ����ǰ��
      }
    }
  }
  return false; // û����Ҫת������
}
bool CheckAndEnd()
{
  if(half_Detect())
  {
    Break();
    drug_change = 1; // ���ñ�־λΪ1����ʾ��Ҫ����ҩ��ģʽ��ת��
    return true; // ��Ҫ������ǰģʽ
  }
  return false; // û����Ҫ���������
}

void RED_up()
{
			HAL_GPIO_WritePin(RED_GPIO_Port, RED_Pin, GPIO_PIN_SET);
}
void RED_down()
{
  HAL_GPIO_WritePin(RED_GPIO_Port, RED_Pin, GPIO_PIN_RESET);
}
void GREEN_up()
{
      HAL_GPIO_WritePin(GREEN_GPIO_Port, GREEN_Pin, GPIO_PIN_SET);
}
void GREEN_down()
{
  HAL_GPIO_WritePin(GREEN_GPIO_Port, GREEN_Pin, GPIO_PIN_RESET);
}
void YELLOW_up()
{
  HAL_GPIO_WritePin(YELLOW_GPIO_Port, YELLOW_Pin, GPIO_PIN_SET);
}
void YELLOW_down()
{
  HAL_GPIO_WritePin(YELLOW_GPIO_Port, YELLOW_Pin, GPIO_PIN_RESET);
}

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
