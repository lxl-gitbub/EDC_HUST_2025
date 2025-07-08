#include "Initialize.h"

Motor Le, Ri;  // Declare motors for left and right
CarKinematics car_kinematics; // Declare car kinematics structure

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
    
    Le = *L;  // Assign the left motor to Le
    Ri = *R;  // Assign the right motor to Ri
    // Set the initial speed to default
}

void LMotorSet(MOVETYPE type, uint16_t duty)
{
    Motor_UI_Set(type, duty, &Le);
}
void RMotorSet(MOVETYPE type, uint16_t duty)
{
    Motor_UI_Set(type, duty, &Ri);
}

float getYaw()
{
    // Get the yaw angle from the MS601M sensor
    atk_ms601m_attitude_data_t attitude_dat;
    atk_ms601m_get_attitude(&attitude_dat, 10);
    return attitude_dat.yaw; // Return the yaw angle
}

float getWz()
{
    // Get the angular velocity (wz) from the MS601M sensor
    atk_ms601m_gyro_data_t gyro_dat;
		atk_ms601m_accelerometer_data_t accelerometer_dat;
    atk_ms601m_get_gyro_accelerometer(&gyro_dat, &accelerometer_dat, 10);
    return gyro_dat.z; // Return the angular velocity around the z-axis
}