#ifndef INITIALIZE_H
#define INITIALIZE_H

// �˶�ѧͷ�ļ�������������ֱ��ת�亯��
#include "kinematics.h"
#include "AllHeader.h"

// Parameters for the encoder
#define PPR 13  // The Pulses Per Revolution of encoder

// Parameters for the motor
#define REDUCE 20
#define FULL_SPEED_RPM 300

// Parameters for the wheel
#define WHEEL_DIAMETER 48 // The diameter of the wheel in mm
#define WHEEL_DIS      130

// ====================��ʼ�����������궨��====================
// �������д���ĳ�ʼ������

// ���ֵ����ʼ������ (��ӦMotorInit(&Right, ...))
#define RIGHT_MOTOR_IN1_PORT    GPIOC
#define RIGHT_MOTOR_IN1_PIN     GPIO_PIN_5
#define RIGHT_MOTOR_IN2_PORT    GPIOB
#define RIGHT_MOTOR_IN2_PIN     GPIO_PIN_1
#define RIGHT_MOTOR_PWM_TIMER   &htim5
#define RIGHT_MOTOR_PWM_CHANNEL TIM_CHANNEL_1
#define RIGHT_MOTOR_INIT_DUTY   0

// ���ֵ����ʼ������ (��ӦMotorInit(&Left, ...))
#define LEFT_MOTOR_IN1_PORT     GPIOC
#define LEFT_MOTOR_IN1_PIN      GPIO_PIN_4
#define LEFT_MOTOR_IN2_PORT     GPIOB
#define LEFT_MOTOR_IN2_PIN      GPIO_PIN_0
#define LEFT_MOTOR_PWM_TIMER    &htim5
#define LEFT_MOTOR_PWM_CHANNEL  TIM_CHANNEL_2
#define LEFT_MOTOR_INIT_DUTY    0

// LRInit�������� (��������ʼ��)
#define LEFT_ENCODER_TIMER      &htim3
#define LEFT_ENCODER_CHANNEL_A  TIM_CHANNEL_2
#define LEFT_ENCODER_CHANNEL_B  TIM_CHANNEL_1
#define RIGHT_ENCODER_TIMER     &htim1
#define RIGHT_ENCODER_CHANNEL_A TIM_CHANNEL_1
#define RIGHT_ENCODER_CHANNEL_B TIM_CHANNEL_2
#define ENCODER_REAL_TIMER      &htim7


#ifdef __cplusplus
extern "C" {
#endif
extern CarState car; // Declare car state for kinematics
extern Data current_data; // Declare current data for kinematics

void MEInit(Motor* L, Motor* R);
void LMotorSet(MOVETYPE type, uint16_t duty);
void RMotorSet(MOVETYPE type, uint16_t duty);

void LSet(int16_t duty);
void RSet(int16_t duty);

void Break(); // Set both motors to break mode
// This function sets both motors to break mode, stopping them quickly

bool isInTheYaw(float targetYaw, float tolerance);

float getYaw(); // ��ȡ��ǰ��ƫ����
float getWz(); // ��ȡ��ǰ�Ľ��ٶ�
// ��ȡ���ݺ�������
void UpdateData(); // ���µ�ǰ����
void UpdateData_Car(); // ��������״̬����

void Back(float theta); // ���˺�����thetaΪĿ��ƫ����

#endif /* INITIALIZE_H */