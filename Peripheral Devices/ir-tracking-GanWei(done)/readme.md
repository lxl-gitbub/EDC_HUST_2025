## ע�⣺

����λ�û�·��ʱ����У׼��У׼��ʽ��ʹ���ĵ�

## �������ã�

- Ӳ��I2C����I2C1����ΪI2Cģʽ
- ����GanWei���ļ�

## ���ߣ�

PB6     ------> I2C1_SCL
PB7     ------> I2C1_SDA

��·��������ñ��scl sda����ñ��ad1��ad0��pull������ñ

## ���룺

```c
/* USER CODE BEGIN PV */
int Digtal[8];
/* USER CODE END PV */

/* USER CODE BEGIN WHILE */
while (1)
{
    IIC_Get_Digtal(Digtal);		//��ȡ���������������,�洢��Digtal[7]������ 
    HAL_Delay(1000);
/* USER CODE END WHILE */
```

