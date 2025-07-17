# Initialize.c ������ʹ��˵��

## ����

Initialize.c ��һ������ͱ�������ʼ����װ�⣬���� TB6612FNG �������оƬ�ͻ������������ÿ��ṩ�˼򻯵ĳ�ʼ���Ϳ��ƽӿڣ�ʹ�õ�����Ƹ��ӱ�ݡ�

## Ӳ������

### ������� (TB6612FNG)
```
���A (����):
- AIN1: PC4 (�������1)
- AIN2: PB0 (�������2)  
- PWMA: PA1 (TIM5_CH2, PWM���)

���B (����):
- BIN1: PC5 (�������1)
- BIN2: PB1 (�������2)
- PWMB: PA0 (TIM5_CH1, PWM���)
```

### ����������
```
���ֱ����� (TIM3):
- E2A: PA6 (TIM3_CH1)
- E2B: PA7 (TIM3_CH2)

���ֱ����� (TIM1):  
- E1A: PE11 (TIM1_CH1)
- E1B: PE9 (TIM1_CH2)

ʵʱ��ʱ��: TIM7 (�����ٶȼ���)
```

## �����ӿ�

### 1. MEInit() - ����ʼ������

```c
void MEInit(Motor* L, Motor* R)
```

**����**: һ����ʼ�����ҵ���ͱ�����ϵͳ

**����**:
- `L`: ���ֵ���ṹ��ָ��
- `R`: ���ֵ���ṹ��ָ��

**����˵��**:
- �Զ���ʼ�����ҵ����GPIO��PWM����
- �Զ���ʼ�����ұ������Ķ�ʱ������
- �����־�Ϊ65mm���������ֱܷ���Ϊ364(13��28)

**ʹ��ʾ��**:
```c
Motor Left, Right;
MEInit(&Left, &Right);

// ���õ������
Motor_Param_Init(&Left, 28, 300, 65);   // ���ٱ�28, ���ת��300rpm, �־�65mm
Motor_Param_Init(&Right, 28, 300, 65);
```

### 2. LMotorSet() - ���ֵ������

```c
void LMotorSet(MOVETYPE type, uint16_t duty)
```

**����**: �������ֵ���˶�

**����**:
- `type`: �˶�����
  - `FOR`: ǰ��
  - `BACK`: ����
  - `BREAK`: �ƶ�ֹͣ
  - `SLIDE`: ����ֹͣ
- `duty`: PWMռ�ձ� (0-1000)

**ʹ��ʾ��**:
```c
LMotorSet(FOR, 300);    // ����ǰ�����ٶ�300
LMotorSet(BACK, 400);   // ���ֺ��ˣ��ٶ�400
LMotorSet(BREAK, 0);    // �����ƶ�ֹͣ
```

### 3. RMotorSet() - ���ֵ������

```c
void RMotorSet(MOVETYPE type, uint16_t duty)
```

**����**: �������ֵ���˶�

**����**: 
- `type`: �˶����� (ͬLMotorSet)
- `duty`: PWMռ�ձ� (0-1000)

**ʹ��ʾ��**:
```c
RMotorSet(FOR, 300);    // ����ǰ�����ٶ�300
RMotorSet(BACK, 400);   // ���ֺ��ˣ��ٶ�400
RMotorSet(BREAK, 0);    // �����ƶ�ֹͣ
```

## �˶�����ʾ��

### �����˶�ģʽ

```c
// 1. ֱ��ǰ��
LMotorSet(FOR, 300);
RMotorSet(FOR, 300);

// 2. ֱ�ߺ���
LMotorSet(BACK, 300);
RMotorSet(BACK, 300);

// 3. ��ת (����ת������ͣ)
LMotorSet(SLIDE, 0);
RMotorSet(FOR, 400);

// 4. ��ת (����ת������ͣ)
LMotorSet(FOR, 400);
RMotorSet(SLIDE, 0);

// 5. ԭ����ת (����ת��)
LMotorSet(BACK, 200);
RMotorSet(FOR, 200);

// 6. ԭ����ת (����ת��)
LMotorSet(FOR, 200);
RMotorSet(BACK, 200);

// 7. ֹͣ
LMotorSet(BREAK, 0);
RMotorSet(BREAK, 0);
```

### ��ϱ��������ٶȿ���

```c
// ��ȡ�������ٶ� (��Ҫ�ڶ�ʱ���ж��е���UpdateAllSpeed)
double left_speed = lSpeed();   // �����ٶ� (m/s)
double right_speed = rSpeed();  // �����ٶ� (m/s)
double center_speed = cSpeed(); // �����ٶ� (m/s)

// �����ٶȷ����������
if(left_speed < 0.2) {
    LMotorSet(FOR, 350);  // ���������ٶ�
}
if(right_speed < 0.2) {
    RMotorSet(FOR, 350);  // ���������ٶ�
}
```

## ��������

### Ԥ�������
```c
#define PPR 13              // ������ÿת������
#define REDUCE 28           // ������ٱ�
#define FULL_SPEED_RPM 300  // ������ת��
#define WHEEL_DIAMETER 65   // �־� (mm)
```

### �ٶȻ���
```c
// �������ֱܷ��� = PPR �� REDUCE = 13 �� 28 = 364 ����/ת
// ���ܳ� = �� �� WHEEL_DIAMETER = �� �� 65 �� 204.2 mm
// �ٶ�(m/s) = (����Ƶ�� / 364) �� 0.2042
```

## ע������

1. **��ʼ��˳��**: �����ȵ���MEInit()���ٵ���Motor_Param_Init()
2. **��ʱ������**: TIM5��������ΪPWM�����TIM1/TIM3����Ϊ������ģʽ��TIM7����Ϊ��ʱ��
3. **����������**: ��Ҫ��TIM7�ж��е���`UpdateAllSpeed(&htim7)`�������ٶ�����
4. **�ٶȷ�Χ**: PWM duty��Χ0-1000����Ӧ0-100%ռ�ձ�
5. **������**: ����ʵ�ʽ��߿�����Ҫ����ǰ��/���˷���

## ����ʹ��ʾ��

```c
#include "Initialize.h"

Motor Left, Right;

int main(void)
{
    // HAL���ʼ��...
    
    // ����ͱ�������ʼ��
    MEInit(&Left, &Right);
    Motor_Param_Init(&Left, 28, 300, 65);
    Motor_Param_Init(&Right, 28, 300, 65);
    
    while(1)
    {
        // ǰ��2��
        LMotorSet(FOR, 300);
        RMotorSet(FOR, 300);
        HAL_Delay(2000);
        
        // ��ת1��
        LMotorSet(SLIDE, 0);
        RMotorSet(FOR, 400);
        HAL_Delay(1000);
        
        // ֹͣ1��
        LMotorSet(BREAK, 0);
        RMotorSet(BREAK, 0);
        HAL_Delay(1000);
        
        // ����ٶ���Ϣ
        printf("Left: %.3f m/s, Right: %.3f m/s\n", lSpeed(), rSpeed());
    }
}

// ��ʱ���жϻص� (TIM7)
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim == &htim7) {
        UpdateAllSpeed(&htim7);  // ���±������ٶ�
    }
}
```

����������TB6612���������ʹ�ã��ṩ�������Ľӿ�������˫�ֲ���С������