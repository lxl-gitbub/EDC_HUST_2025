# TB6612FNG ���������˵��

�������ڻ��� STM32 �� TB6612FNG �������оƬ���ƣ�֧�ֵ����ת����ת��ɲ�������еȶ��ֹ���ģʽ������ͨ�� PWM ռ�ձȵ����ٶȡ�

---
## STM32CubeMX (IOC) ����˵��

��ʹ�� TB6612FNG ���������ǰ����ͨ�� STM32CubeMX ���ߣ����� `.ioc` �����ļ�������������ĳ�ʼ�����á���Ҫ������

- �������ڵ�����Ƶ� GPIO ����Ϊ���ģʽ���� IN1��IN2����
- ����һ����ʱ���� PWM ͨ�������ڵ��ڵ���ٶȡ�
- ȷ��ʱ�ӡ����Ÿ��õ�������ʵ��Ӳ������һ�¡�

**����ʾ����**

����ʹ�� STM32F103C8T6����� A �� IN1��IN2 �ֱ����ӵ� PA1��PA2��PWM �ź������ TIM1_CH1��PA8����

1. �� CubeMX �ġ�Pinout & Configuration�����棬�� PA1��PA2 ����Ϊ GPIO_Output��PA8 ����Ϊ TIM1_CH1��
2. �ڡ�Configuration���У����� TIM1 Ϊ PWM Generation Channel 1��Prescaler �� Period ������Ҫ�������� Period ��Ϊ 1000����
3. ���ɴ���󣬼����� `main.c` �е��ñ��⺯�����е�����ơ�

## ��Ҫ�����÷�

### 1. MotorInit

```c
void MotorInit(
    Motor* M,
    GPIO_TypeDef* p_port, uint16_t p_pin,
    GPIO_TypeDef* n_port, uint16_t n_pin,
    TIM_HandleTypeDef* pwm_timer, uint32_t pwm_channel,
    uint16_t current_duty
);
```

**����˵����**  
��ʼ������ṹ�壬�������������š�PWM ��ʱ������ʼռ�ձȣ������������Ϊ���ߣ�SLEEP��ģʽ��

**����˵����**
- `M`��ָ�� Motor �ṹ���ָ��
- `p_port`��`p_pin`������������ŵ� GPIO ������źţ��� GPIOA, GPIO_PIN_1��
- `n_port`��`n_pin`������������ŵ� GPIO ������ź�
- `pwm_timer`��PWM ��ʱ�����ָ��
- `pwm_channel`��PWM ͨ���ţ��� TIM_CHANNEL_1��
- `current_duty`����ʼ PWM ռ�ձȣ�1~1000��

**ʹ��ʾ����**
```c
Motor motorA;
MotorInit(&motorA, GPIOA, GPIO_PIN_1, GPIOA, GPIO_PIN_2, &htim1, TIM_CHANNEL_1, 500);
```

---

### 2. MotorSet

```c
void MotorSet(MOVETYPE type, uint16_t duty, Motor* M);
```

**����˵����**  
���õ�����˶�ģʽ�� PWM ռ�ձȣ�ʵ��ǰ�������ˡ�ɲ�������еȹ��ܡ�

**����˵����**
- `type`���˶�ģʽ��FOR ǰ����BACK ���ˣ�BREAK ɲ����SLIDE ���У�SLEEP ���ߣ�
- `duty`��PWM ռ�ձȣ�1~1000��
- `M`��ָ�� Motor �ṹ���ָ��

**ʹ��ʾ����**
```c
MotorSet(FOR, 800, &motorA);   // �����ת���ٶȽϿ�
MotorSet(BACK, 400, &motorA);  // �����ת���ٶȽ���
MotorSet(BREAK, 0, &motorA);   // ����ƶ�
MotorSet(SLIDE, 0, &motorA);   // �������
```

---

## ����˵��

- �ṹ�� `Motor` ��ö�� `MOVETYPE` ���� `tb6612fng.h` �ж��塣
- PWM ռ�ձ���ʵ���ٶȷ����Թ�ϵ���������ʵ��������� duty ֵ��
- �������ָ��Ϊ NULL��������ֱ�ӷ��أ��������κβ�����

---

## ע������

- ʹ��ǰ����� `tb6612fng.h` ����ȷ��ʼ��������裨GPIO����ʱ���ȣ���
- ����������ż� PWM ͨ������Ӳ������һ�¡�
- ������ԣ���ȡ��ע�ʹ����е� UART �����䣬���滻Ϊʵ�ʴ��ھ����
- duty ���������� 1~1000 ��Χ�ڣ����ⳬ�� PWM ��ʱ���ļ�����Χ��

---

�������ʻ����һ����չ����ο�Դ��ע�ͻ���ϵ�����ߡ�