#include "kinematics.h"

uint32_t time = 0;

void PID_Init(PIDdata *pid) {
    if (pid == NULL) return; // Check for null pointer
    pid->error = 0.0f;
    pid->sum = 0.0f;
    pid->difference = 0.0f;
}
void PID_Update(PIDdata *pid, float target, float current, float dt) {
    if (pid == NULL || dt <= 0.0f) return; // Check for null pointer and valid dt
    pid->error = target - current; // Calculate error
    pid->sum += pid->error * dt; // Integral term
    pid->difference = (pid->error - pid->difference) / dt; // Derivative term
}
float PID_Compute(PIDdata *pid, float Kp, float Ki, float Kd) {
    if (pid == NULL) return 0.0f; // Check for null pointer
    return Kp * pid->error + Ki * pid->sum + Kd * pid->difference; // PID output
}

// Initializes the car state to default values
void CarState_Init(CarState *state) {
    if (state == NULL) return; // Check for null pointer
    state->pose.x = 0.0f;
    state->pose.y = 0.0f;
    state->pose.theta = 0.0f;
    state->pose.initial_theta = getYaw(); // Initialize with current yaw angle
    state->speed.linear_velocity = 0.0f;
    state->speed.angular_velocity = 0.0f;
    state->wheel_speed.left_wheel_speed = 0.0f;
    state->wheel_speed.right_wheel_speed = 0.0f;
}
// Updates the car state based on wheel speeds and time delta
void CarState_Update(CarState *state, Data d) {
    if (state == NULL) return; // Check for null pointer and valid dt
    // Calculate the average wheel speed
    state->speed = d.speed;
    state->pose.theta = d.yaw; // Update initial theta with current yaw
    state->pose.x += d.speed.linear_velocity * cos(state->pose.theta) * d.dt; // Update x position
    state->pose.y += d.speed.linear_velocity * sin(state->pose.theta) * d.dt; // Update y position
}

int isGetPose(CarState *state, Pose *pose)
{
    
}

Data getData()
{
    Data data;
    data.speed.angular_velocity = getWz(); // 获取当前的角速度
    data.speed.linear_velocity = cSpeed();
    data.yaw = getYaw(); // 获取当前的偏航角

    uint32_t now = HAL_GetTick();
    data.dt = now - time; // 计算时间间隔
    time = now; // 更新上次获取数据的时间

    return data;
}
