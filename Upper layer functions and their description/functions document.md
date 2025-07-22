# EDC�˶�ѧ���ƿ⺯��˵���ĵ�
**�ȿ�����**

## ����
- ֱ���Լ�תԲ����straight��runcircle
- ��ȡ���������ٶȣ����ٶȣ����꣬�Լ���ԽǶ���ȫ�ֱ���car 
**ע��������������д��while��1��ѭ������д��UpdateData_Car������ͬʱ���ָ�Ƶ�����Լ������**
- ��motor������LSet(int) RSet(int) Break()��ɲ����
**while(1)ѭ��֮ǰ����MEinit()**
- �Ѿ�ʵ���˵ײ�pid���ƣ������Ͽ���ʵ��Ը��ӵ��˶�


## Ŀ¼
- [1. ��ʼ������ (Initialize.h)](#1-��ʼ������-initializeh)
- [2. �˶�ѧ���ƺ��� (kinematics.h)](#2-�˶�ѧ���ƺ���-kinematicsh)
- [3. ���ݽṹ˵��](#3-���ݽṹ˵��)
- [4. ʹ��ʾ��](#4-ʹ��ʾ��)

---

## 1. ��ʼ������ (Initialize.h)
- �⺯��Ŀ�ģ�Ϊÿһ�����ṩһ��ʾ�����ӿڣ�ͬʱд�������������Ҫ�õ��ĺ���

### 1.1 ����ͱ�������ʼ��

#### `void MEInit(Motor* L, Motor* R)`
**���ܣ�** ��ʼ�����ҵ���ͱ�����ϵͳ  
**������**
- `L`: �����ṹ��ָ��
- `R`: �ҵ���ṹ��ָ��

**ʹ��˵����**
- ��������������ʼʱ����
- �Զ���ɵ��PWM����������ʱ����С��״̬�ĳ�ʼ��
- ���ú�ȫ�ֱ���`car`��`current_data`����

```c
Motor Left, Right;
MEInit(&Left, &Right); // ��ʼ������ͱ�����
```

---

### 1.2 ������ƺ���

#### `void LSet(int16_t duty)` / `void RSet(int16_t duty)`
**���ܣ�** ������/���ֵ��PWMռ�ձ�  
**������**
- `duty`: PWMռ�ձȣ���Χ[-1000, 1000]
  - ��ֵ��ǰ��
  - ��ֵ������
  - 0��ֹͣ

**�ص㣺**
- �Զ��жϷ���FOR/BACK��
- �Զ��޷�����1000
- �Ƽ��ĵײ������ƽӿ�

```c
LSet(500);   // ����ǰ����50%ռ�ձ�
RSet(-300);  // ���ֺ��ˣ�30%ռ�ձ�
```

#### `void LMotorSet(MOVETYPE type, uint16_t duty)` / `void RMotorSet(MOVETYPE type, uint16_t duty)`
**���ܣ�** ������/���ֵ���˶����ͺ�PWMռ�ձ�  
**������**
- `type`: �˶����ͣ�FORǰ��/BACK����/BREAK�ƶ���
- `duty`: PWMռ�ձȣ���Χ[0, 1000]

```c
LMotorSet(FOR, 400);    // ����ǰ����40%ռ�ձ�
RMotorSet(BREAK, 0);    // �����ƶ�
```

#### `void Break()`
**���ܣ�** ����ͣ��������ͬʱ�ƶ�  
**ʹ�ó�����** ��Ҫ����ͣ��ʱ����

```c
Break(); // ����ͣ��
```

---

### 1.3 ���������ݻ�ȡ

#### `float getYaw()`
**���ܣ�** ��ȡ��ǰƫ����  
**����ֵ��** ƫ���ǣ���λ�ȣ���Χ[-180, 180]

#### `float getWz()`
**���ܣ�** ��ȡ��ǰ���ٶ�  
**����ֵ��** ��Z����ٶȣ���λ��/��

#### `void UpdateData()`
**���ܣ�** ����ȫ�����ݽṹ`current_data`  
**ʹ��˵����**
- **��������ѭ��ÿ���ڵ���**
- �������ٶȡ����ٶȡ�ƫ���ǡ�ʱ����
- �����˶�ѧ��������������

#### `void UpdateData_Car()`
**���ܣ�** ��������״̬����`car`  
**ʹ��˵����**
- �ڲ�����`UpdateData()`
- �������С��λ������(x,y)�ͳ���

```c
while(1) {
    UpdateData();        // ����ÿ���ڵ���
    // ��
    UpdateData_Car();    // ����λ����Ϣ
    
    // �˶����ƴ���...
}
```

---

### 1.4 ��������

#### `bool isInTheYaw(float targetYaw, float tolerance)`
**���ܣ�** ��鵱ǰƫ�����Ƿ���Ŀ��Ƕȸ���  
**������**
- `targetYaw`: Ŀ��ƫ���ǣ���λ��
- `tolerance`: ��������λ��

**����ֵ��** ����Χ�ڷ���true������false

```c
if(isInTheYaw(90.0f, 3.0f)) {
    // ��ǰ������90��3�ȷ�Χ��
}
```

#### `void Back(float theta)`
**���ܣ�** �򵥺��˺���  
**������**
- `theta`: Ŀ��ƫ��������ڳ�ʼ�Ƕȵ�ƫ�ƣ���λ��

**ʹ��˵����**
- ���˾���̶�0.3�ף��ٶ�0.3m/s
- �ʺϼ򵥵ĺ��˲���

```c
Back(0); // ���ֵ�ǰ�������0.3��
```

---

## 2. �˶�ѧ���ƺ��� (kinematics.h)

### 2.1 PID����

#### `void PID_Init(PIDdata *pid)`
**���ܣ�** ��ʼ��PID���ݽṹ

#### `void PID_Update(PIDdata *pid, float target, float current, float dt)`
**���ܣ�** ����PID��������

#### `float PID_Compute(PIDdata *pid, float Kp, float Ki, float Kd)`
**���ܣ�** ����PID���

---

### 2.2 �����˶�����

#### `Speed PID_Move(float v, float w, short isreload)`
**���ܣ�** PID�ٶȿ��ƺ���  
**������**
- `v`: Ŀ�����ٶȣ���λm/s
- `w`: Ŀ����ٶȣ���λ��/s
- `isreload`: 1-���³�ʼ��PID��0-��������

**����ֵ��** ��ǰʵ���ٶ�

**ʹ��˵����**
- �߼��ٶȿ��ƽӿڣ��Զ�PID����
- ��һ�ε��û��л�Ŀ��ʱ����`isreload=1`
- ����������������`isreload=0`

```c
// ǰ��0.2m/s
PID_Move(0.2f, 0.0f, 1);  // ��ʼ��
while(1) {
    UpdateData();
    PID_Move(0.2f, 0.0f, 0); // ��������
}

// ԭ����ת30��/s
PID_Move(0.0f, 30.0f, 1);  // ��ʼ��
while(1) {
    UpdateData();
    PID_Move(0.0f, 30.0f, 0); // ��������
}
```

#### `bool Straight(float distance, float speed, float target_yaw, DIR dir)`
**���ܣ�** ֱ����ʻ����  
**������**
- `distance`: Ŀ����룬��λ��
- `speed`: Ŀ���ٶȣ���λm/s
- `target_yaw`: Ŀ��ƫ���ǣ���λ��
- `dir`: ����FORWARDǰ��/BACKWARD���ˣ�

**����ֵ��** ��ʱ����false���������ʱ�ɷ���true��

**ʹ��˵����**
- �Զ�����ֱ�ߺ�Ŀ��Ƕ�
- ��Ҫ����ѭ���г�������
- ���þ�����ֺͽǶ�����

```c
float start_yaw = getYaw();
while(1) {
    UpdateData();
    Straight(1.0f, 0.2f, start_yaw, FORWARD); // ǰ��1��
    // �ɸ��ݷ���ֵ�ж��Ƿ����
}
```

#### `bool runCircle(float radius, float speed, float angle, DIR dir)`
**���ܣ�** Բ���˶�����  
**������**
- `radius`: ת��뾶����λ��
- `speed`: ���ٶȣ���λm/s
- `angle`: ת���Ƕȣ���λ��
- `dir`: ת��LEFT��ת/RIGHT��ת��

**����ֵ��** ��ʱ����false���Ƕ����ʱ�ɷ���true��

**ʹ��˵����**
- ��ָ���뾶��Բ���˶�
- �Զ�������ٶ�
- ��Ҫ����ѭ���г�������

```c
while(1) {
    UpdateData();
    runCircle(0.5f, 0.2f, 90.0f, LEFT); // �뾶0.5����ת90��
    // �ɸ��ݷ���ֵ�ж��Ƿ����
}
```

---

### 2.3 ��������

#### `WheelSpeed SpeedToWheelSpeed(Speed speed)`
**���ܣ�** �������ٶ�ת��Ϊ�������ٶ�

#### `float sumTheta(float theta1, float theta2)`
**���ܣ�** �Ƕ���Ӳ���һ����[-180, 180]

---

## 3. ���ݽṹ˵��

### `Pose`
```c
typedef struct{
    float x;
    float y;
    float theta; //ƫ�ƽǣ����ϴε��õ�MEinitʱΪ0�㣬��ʱ��Ϊ������
}Pose;
```

### `Speed`
```c
typedef struct{
    float linear_velocity;  // ���ٶȣ�m/s
    float angular_velocity; // ���ٶȣ���/s
}Speed;
```

### `CarState`
```c
typedef struct{
    Pose pose;              // λ�úͷ���
    Speed speed;            // ��ǰ�ٶ�
    WheelSpeed wheel_speed; // �������ٶ�
}CarState;
```

### `Data`
```c
typedef struct{
    Speed speed;  // ��ǰ�ٶ�
    float yaw;    // ��ǰƫ����
    float dt;     // ʱ����
}Data;
```

---

## 4. ʹ��ʾ��

### 4.1 ������ʼ��
```c
int main(void)
{
    // HAL��ʼ��...
    
    Motor Left, Right;
    MEInit(&Left, &Right);
    
    while(1) {
        UpdateData(); // ����ÿ���ڵ���
        
        // �˶����ƴ���...
    }
}
```

### 4.2 ��ǰ������
```c
while(1) {
    UpdateData();
    
    LSet(300);  // ����ǰ��30%
    RSet(300);  // ����ǰ��30%
    HAL_Delay(2000);
    
    LSet(-200); // ���ֺ���20%
    RSet(-200); // ���ֺ���20%
    HAL_Delay(2000);
}
```

### 4.3 PID�ٶȿ���
```c
PID_Move(0.2f, 0.0f, 1); // ��ʼ����ǰ��0.2m/s

while(1) {
    UpdateData();
    Speed current = PID_Move(0.2f, 0.0f, 0);
    
    // ���Ի�ȡ��ǰʵ���ٶȽ��м��
    printf("Current speed: %.3f m/s\n", current.linear_velocity);
}
```

### 4.4 ֱ�ߺ�ת�����
```c
float start_yaw = getYaw();

while(1) {
    UpdateData();
    
    // ��һ�׶Σ�ֱ��1��
    if(!Straight(1.0f, 0.2f, start_yaw, FORWARD)) {
        continue;
    }
    
    // �ڶ��׶Σ���ת90��
    if(!runCircle(0.3f, 0.15f, 90.0f, LEFT)) {
        continue;
    }
    
    // ������ж���
    Break();
    break;
}
```

---
