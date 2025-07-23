# Hall_Encoder ����������ģ��

��ģ��Ϊ STM32 ƽ̨�µĻ������������ٿ⣬������˫�ֲ���С���ȳ�����ͨ����ʱ���ͱ������ӿڣ�ʵʱ��ȡ���٣�֧�ֶ�����������������ɵ�ʵ����Ŀ�С�

---

## ���ܼ��

- ֧�ֶ�·���������٣���� 10 ·��
- ͨ����ʱ���ж��Զ������ٶ�
- �ٶȵ�λΪ m/s������������ǰ��/����
- �ṩ�����õĳ�ʼ���Ͳ�ѯ�ӿ�

---

## ��Ҫ����˵��

### 1. `void EncoderInit(TIM_HandleTypeDef *Timer, uint32_t chan1, uint32_t chan2, TIM_HandleTypeDef *realTimer);`

**����**��  
��ʼ��һ������������ͨ����������ض�ʱ����ͨ���������� PWM ����ʱ�жϡ�

**����**��
- `Timer`����������ʱ�����
- `chan1`��`chan2`��������ͨ����ţ��� `TIM_CHANNEL_1`��`TIM_CHANNEL_2`��
- `realTimer`�����ڲ��ٵĶ�ʱ�����

**ʹ��ʾ��**��
```c
EncoderInit(&htim2, TIM_CHANNEL_1, TIM_CHANNEL_2, &htim7);
EncoderInit(&htim1, TIM_CHANNEL_1, TIM_CHANNEL_2, &htim7);
```
> ��������ֱ��ʼ�������������������ٶ�ʱ����Ϊ TIM7��
> �����ʹ��Դ���룬�Ұ����ͺ�Ϊstm32h743vitxϵ�а��ӣ��ɽ�PE11������a��������PE9������b��������PA0������a��������PA1��b��������������ʵ��ǰ�����˷�����ȷ�Ĳ���

---

### 2. `void UpdateAllSpeed(TIM_HandleTypeDef *reload_tim);`

**����**��  
���������ѳ�ʼ�����������ٶȡ�ͨ���ڶ�ʱ������ж��е��ã�ʵ���Զ����١�

**�����÷�**�����ڶ�ʱ���жϻص������У���
```c
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    UpdateAllSpeed(htim);
}
```
> �Ƽ������ٶ�ʱ������ TIM7�����жϷ������е��� `UpdateAllSpeed`��ʵ�ֶ�ʱ�Զ����١�

---

### 3. `double getSpeed(int index);`

**����**��  
��ȡָ���������ĵ�ǰ�ٶȣ���λΪ m/s��`index` Ϊ��ʼ��˳��0 ��ʾ��һ������������

**ʾ��**��
```c
double left_speed = getSpeed(0);
double right_speed = getSpeed(1);
```

---

## �����������

- `int getTIMx_DetaCnt(TIM_HandleTypeDef *htim);`  
  ��ȡ��������ʱ�����ϴε��ú�ļ����仯���������ü�������

- `void UpdateSpeed(int i, TIM_HandleTypeDef *reload_tim);`  
  ����ָ�����������ٶȣ�һ�������ֶ����ã��� `UpdateAllSpeed` ������

- `uint32_t ReloadTime(TIM_TypeDef* tim);`  
  ���㶨ʱ����ʵ�����Ƶ�ʣ������ٶȻ��㡣

---

## ע������

- ʹ��ǰ��ȷ����ض�ʱ����GPIO ���� CubeMX ���ֶ���ʼ����ɡ�
- ֧�ֶ����������ʼ��˳����� `getSpeed(index)` ��������
- �Ƽ��� `UpdateAllSpeed` ���ڲ��ٶ�ʱ�����жϻص��У���֤����ʵʱ�ԡ�
- �ٶȼ���������ʱ�������ͱ����������������ʵ��Ӳ�����õ����궨�塣

---

������ϸ�˽����ӿں��÷�����ο�Դ��ע�ͻ���ϵ���ߡ�