#include "Initialize.h"

Motor Le, Ri;  // Declare motors for left and right
CarState car; // Declare car state for kinematics

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

    CarState_Init(&car); // Initialize the car state
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

void LSet(int16_t duty)
{
    MOVETYPE type = FOR; // Default to forward motion
    
    if(duty < 0) {
        type = BACK; // If duty is negative, set to backward motion
        duty = -duty; // Convert to positive duty cycle
    }
    if(duty > 1000) {
        duty = 1000; // Limit the duty cycle to a maximum of 1000
    }
    LMotorSet(type, duty); // Set the left motor with the specified type and duty cycle
}

void RSet(int16_t duty)
{
    MOVETYPE type = FOR; // Default to forward motion
    
    if(duty < 0) {
        type = BACK; // If duty is negative, set to backward motion
        duty = -duty; // Convert to positive duty cycle
    }
    if(duty > 1000) {
        duty = 1000; // Limit the duty cycle to a maximum of 1000
    }
    RMotorSet(type, duty); // Set the right motor with the specified type and duty cycle
}

void Break()
{
    LMotorSet(BREAK, 0); // Set both motors to break mode
    RMotorSet(BREAK, 0);
}
void XAligning()//对齐x轴
{
    float targetYaw = car.pose.initial_theta; // 目标偏航角为初始方向角
    float currentYaw = getYaw(); // 获取当前偏航角
    if(sumTheta(currentYaw, -targetYaw) > 0) {// 如果当前偏航角大于目标偏航角{
        LMotorSet(FOR, 100); // 左轮前进
        RMotorSet(BACK, 100); // 右轮后退
    } else if(sumTheta(currentYaw, -targetYaw) < 0) { // 如果当前偏航角小于目标偏航角
        LMotorSet(BACK, 100); // 左轮后退
        RMotorSet(FOR, 100); // 右轮前进
    }
    while(fabs(getYaw() - targetYaw) > 1) // 当偏航角与目标角度的差值大于0.5时
    {
        HAL_Delay(10); // 等待10毫秒
    }
    LMotorSet(BREAK, 0); // 左轮停止
    RMotorSet(BREAK, 0); // 右轮停止
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
