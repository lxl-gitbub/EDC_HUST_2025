#include "Initialize.h"

Motor Left, Right;  // Declare motors for left and right

void MEInit(Motor* L, Motor* R)
{
    Motor_UI_Init(L, LEFT_MOTOR_IN1_PORT, LEFT_MOTOR_IN1_PIN,
        LEFT_MOTOR_IN2_PORT, LEFT_MOTOR_IN2_PIN,
        LEFT_MOTOR_PWM_TIMER, LEFT_MOTOR_PWM_CHANNEL, LEFT_MOTOR_INIT_DUTY);
    Motor_UI_Init(R, RIGHT_MOTOR_IN1_PORT, RIGHT_MOTOR_IN1_PIN,
        RIGHT_MOTOR_IN2_PORT, RIGHT_MOTOR_IN2_PIN,
        RIGHT_MOTOR_PWM_TIMER, RIGHT_MOTOR_PWM_CHANNEL, RIGHT_MOTOR_INIT_DUTY);

    LRInit(LEFT_ENCODER_TIMER, LEFT_ENCODER_CHANNEL_A, LEFT_ENCODER_CHANNEL_B,
           RIGHT_ENCODER_TIMER, RIGHT_ENCODER_CHANNEL_A, RIGHT_ENCODER_CHANNEL_B,
           WHEEL_DIAMETER, PPR * REDUCE, ENCODER_REAL_TIMER ); // Initialize the encoders with wheel length and pulses per revolution
    // Set the initial speed to default
}

void LMotorSet(MOVETYPE type, uint16_t duty)
{
    Motor_UI_Set(type, duty, &Left);
}
void RMotorSet(MOVETYPE type, uint16_t duty)
{
    Motor_UI_Set(type, duty, &Right);
}

float getYaw()
{
    // Get the yaw angle from the MS601M sensor
    atk_ms601m_attitude_data_t attitude_dat;
    atk_ms601m_get_attitude(&attitude_dat, 100);
    return attitude_dat.yaw; // Return the yaw angle
}