## ע�⣺

����λ�û�·��ʱ����У׼��У׼��ʽ��ʹ���ĵ�

## �������ã�

- Ӳ��I2C����I2C1����ΪI2Cģʽ
- ����GanWei���ļ�

## ���ߣ�

PB6     ------> I2C1_SCL
PB7     ------> I2C1_SDA

## ���룺

```c
/* USER CODE BEGIN PV */
unsigned char Digtal;
/* USER CODE END PV */

/* USER CODE BEGIN WHILE */
while (1)
{
    unsigned char rx_buff[256]={0};
    Digtal=IIC_Get_Digtal();		//��ȡ������ģ�������
    int buff_len = sprintf((char *)rx_buff,"Digtal %d-%d-%d-%d-%d-%d-%d-%d\r\n",(Digtal>>0)&0x01,(Digtal>>1)&0x01,(Digtal>>2)&0x01,(Digtal>>3)&0x01,(Digtal>>4)&0x01,(Digtal>>5)&0x01,(Digtal>>6)&0x01,(Digtal>>7)&0x01);
    HAL_UART_Transmit(&huart1,(uint8_t *)rx_buff, buff_len, HAL_MAX_DELAY); 
    HAL_Delay(1000);
/* USER CODE END WHILE */
```

