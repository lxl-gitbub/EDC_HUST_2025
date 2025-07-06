## ע�⣺

- ÿ��ʹ������Ƚ���У׼��У׼��Ҫ����λ����ATX-IMU1.0.4)

## Ӳ�����ӣ�

- USART2����������
- USART1���Ӵ�������

## �������ã�

- USART2��Ҫ��global interrupt��������115200
- TIM6����Ϊ���躯��delay_us_hal��ʹ��ʱ�ӣ�1us����prescalerΪ240-1
- Driver���е���BSP�ļ��У��ں����ļ�

## ���룺
- �����÷�
```c
    atk_ms601m_get_attitude(&attitude_dat, 100);                            /* ��̬�� */
    atk_ms601m_get_gyro_accelerometer(&gyro_dat, &accelerometer_dat, 100);  /* ���ٶȺͼ��ٶ� */
```
		������������������ֵ��ʵ���ϴ���Ľṹ���ֵ���и���

- main.c

```c
/* USER CODE BEGIN PV */
atk_ms601m_attitude_data_t attitude_dat;
atk_ms601m_gyro_data_t gyro_dat;
atk_ms601m_accelerometer_data_t accelerometer_dat;
/* USER CODE END PV */


  /* USER CODE BEGIN 2 */
	atk_ms601m_init(115200);
  /* USER CODE END 2 */

  /* USER CODE BEGIN WHILE */
  while (1)
  {
	/* ��ȡ���������� */
    atk_ms601m_get_attitude(&attitude_dat, 100);                            /* ��̬�� */
    atk_ms601m_get_gyro_accelerometer(&gyro_dat, &accelerometer_dat, 100);  /* ���ٶȺͼ��ٶ� */
		
		
	//���������̬������attitude_dat.roll, attitude_dat.pitch, attitude_dat.yaw
	//������ý��ٶ�����gyro_dat.x, gyro_dat.y, gyro_dat.z
	//������ü��ٶ�����accelerometer_dat.x, accelerometer_dat.y, accelerometer_dat.z)

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/* USER CODE BEGIN 4 */
void delay_us_hal(uint16_t nus)
{
    __HAL_TIM_SET_COUNTER(&htim6, 0);
    HAL_TIM_Base_Start(&htim6);       
    while (__HAL_TIM_GET_COUNTER(&htim6) < nus); 
    HAL_TIM_Base_Stop(&htim6);        
}
/* USER CODE END 4 */
```

- main.h

  ```c
  /* USER CODE BEGIN EFP */
  void delay_us_hal(uint16_t nus);
  /* USER CODE END EFP */
  ```

  

- **��Ҫ��stm32h7xx_it.c�н�void USARTX_IRQHandler(void)����ע�ͷ�ֹ��ͻ**

- **��Ҫ��atk_ms601m_uart.h�н����Ŷ����滻�����ô�������**

