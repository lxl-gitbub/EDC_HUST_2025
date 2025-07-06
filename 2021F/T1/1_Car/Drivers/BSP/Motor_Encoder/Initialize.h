#ifndef INITIALIZE_H
#define INITIALIZE_H

#include "AllHeader.h"

// Parameters for the encoder
#define PPR 13  // The Pulses Per Revolution of encoder

// Parameters for the motor
#define REDUCE 28
#define FULL_SPEED_RPM 300

// Parameters for the wheel
#define WHEEL_DIAMETER 65 // The diameter of the wheel in mm

// ====================初???化函数参数宏定??====================
// 基于现有代码的初始化参数

// 右轮电机初???化参数 (对应MotorInit(&Right, ...))
#define RIGHT_MOTOR_IN1_PORT    GPIOC
#define RIGHT_MOTOR_IN1_PIN     GPIO_PIN_5
#define RIGHT_MOTOR_IN2_PORT    GPIOB
#define RIGHT_MOTOR_IN2_PIN     GPIO_PIN_1
#define RIGHT_MOTOR_PWM_TIMER   &htim5
#define RIGHT_MOTOR_PWM_CHANNEL TIM_CHANNEL_1
#define RIGHT_MOTOR_INIT_DUTY   0

// 左轮电机初???化参数 (对应MotorInit(&Left, ...))
#define LEFT_MOTOR_IN1_PORT     GPIOC
#define LEFT_MOTOR_IN1_PIN      GPIO_PIN_4
#define LEFT_MOTOR_IN2_PORT     GPIOB
#define LEFT_MOTOR_IN2_PIN      GPIO_PIN_0
#define LEFT_MOTOR_PWM_TIMER    &htim5
#define LEFT_MOTOR_PWM_CHANNEL  TIM_CHANNEL_2
#define LEFT_MOTOR_INIT_DUTY    0

// LRInit函数参数 (编码器初始化)
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

void MEInit(Motor* L, Motor* R);
void LMotorSet(MOVETYPE type, uint16_t duty);
void RMotorSet(MOVETYPE type, uint16_t duty);

#endif /* INITIALIZE_H */