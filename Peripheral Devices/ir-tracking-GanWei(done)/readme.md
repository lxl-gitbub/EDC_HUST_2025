## 注意：

更换位置或路线时，需校准，校准方式见使用文档

## 工程配置：

- 硬件I2C：打开I2C1设置为I2C模式
- 引入GanWei库文件

## 接线：

PB6     ------> I2C1_SCL
PB7     ------> I2C1_SDA

## 代码：

```c
/* USER CODE BEGIN PV */
unsigned char Digtal;
/* USER CODE END PV */

/* USER CODE BEGIN WHILE */
while (1)
{
    unsigned char rx_buff[256]={0};
    Digtal=IIC_Get_Digtal();		//获取传感器模拟量结果
    int buff_len = sprintf((char *)rx_buff,"Digtal %d-%d-%d-%d-%d-%d-%d-%d\r\n",(Digtal>>0)&0x01,(Digtal>>1)&0x01,(Digtal>>2)&0x01,(Digtal>>3)&0x01,(Digtal>>4)&0x01,(Digtal>>5)&0x01,(Digtal>>6)&0x01,(Digtal>>7)&0x01);
    HAL_UART_Transmit(&huart1,(uint8_t *)rx_buff, buff_len, HAL_MAX_DELAY); 
    HAL_Delay(1000);
/* USER CODE END WHILE */
```

