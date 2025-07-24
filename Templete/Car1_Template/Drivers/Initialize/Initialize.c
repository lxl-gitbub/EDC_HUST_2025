#include "Initialize.h"


Motor Le, Ri;  // Declare motors for left and right
CarState car; // Declare car state for kinematics
Data current_data;
int Digital[8];


void MECInit()
{
    Motor_UI_Init(&Le, LEFT_MOTOR_IN1_PORT, LEFT_MOTOR_IN1_PIN,
        LEFT_MOTOR_IN2_PORT, LEFT_MOTOR_IN2_PIN,
        LEFT_MOTOR_PWM_TIMER, LEFT_MOTOR_PWM_CHANNEL, LEFT_MOTOR_INIT_DUTY);
    Motor_UI_Init(&Ri, RIGHT_MOTOR_IN1_PORT, RIGHT_MOTOR_IN1_PIN,
        RIGHT_MOTOR_IN2_PORT, RIGHT_MOTOR_IN2_PIN,
        RIGHT_MOTOR_PWM_TIMER, RIGHT_MOTOR_PWM_CHANNEL, RIGHT_MOTOR_INIT_DUTY);

    LRInit(LEFT_ENCODER_TIMER, LEFT_ENCODER_CHANNEL_A, LEFT_ENCODER_CHANNEL_B,
           RIGHT_ENCODER_TIMER, RIGHT_ENCODER_CHANNEL_A, RIGHT_ENCODER_CHANNEL_B,
           WHEEL_DIAMETER, PPR * REDUCE, ENCODER_REAL_TIMER ); // Initialize the encoders with wheel length and pulses per revolution
    
   CarState_Init(&car); // Initialize the car state
    // Set the initial speed to default
    car.pose.initial_theta = CalibrateYawOffset(); // Calibrate the initial yaw offset
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

float CalibrateYawOffset()
{
    float sum = 0;
    int N = 100;
    for(int i = 0; i < N; ++i) {
        sum += getYaw();
        HAL_Delay(10);  
    }
    float yaw_offset = sum / N;
    // Calculate the average yaw offset over N samples
    // This helps to reduce noise and improve accuracy
    return yaw_offset; // Return the calibrated yaw offset
}

void UpdateData()
{
    const float speedCorrection = 1.03f; // Speed correction factor
    static uint32_t last_time = 0; // Last update time
    uint32_t now = HAL_GetTick(); // Get the current time
    static bool first_run = true; // Flag to indicate the first run
    current_data.speed.linear_velocity = cSpeed() * speedCorrection; // Get the current speed
    current_data.speed.angular_velocity = getWz(); // Get the current angular velocity
    current_data.yaw = getYaw(); // Get the current yaw angle
    IIC_Get_Digital(Digital);
    if(first_run)
    {
        first_run = false; // Set the flag to false after the first run
        current_data.dt = 0; // Initialize dt on the first run
    }
    else
    {
        current_data.dt = (now - last_time) * 1e-3f; // Calculate the time difference in seconds
    }
    if(current_data.dt <= 0.0f) current_data.dt = 0.01f; // Ensure dt is not zero to avoid division by zero 
    last_time = now; // Initialize last_time on the first run

    return; // Exit the function
}

void UpdateData_Car()
{
    // Update the car state with the current data
    UpdateData(); // Update the current data
    CarState_Update(&car, current_data); // Update the car state with the current data
}



/* Check if the current yaw is within the specified tolerance of the target yaw */


