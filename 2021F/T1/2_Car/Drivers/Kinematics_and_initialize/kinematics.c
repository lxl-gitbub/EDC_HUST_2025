#include "kinematics.h"

void PID_Init(PIDdata *pid) {
    if (pid == NULL) return; // Check for null pointer
    pid->error = 0.0f;
    pid->sum = 0.0f;
    pid->difference = 0.0f;
}
void PID_Update(PIDdata *pid, float target, float current, float dt) {
    if (pid == NULL || dt <= 0.0f) return; // Check for null pointer and valid dt
		float preerror = pid->error;
    pid->error = target - current; // Calculate error
    pid->sum += pid->error * dt; // Integral term
    pid->difference = (pid->error - preerror) / dt; // Derivative term
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
    static uint32_t last_time = 0; // Last update time
    uint32_t now = HAL_GetTick(); // Get current time
    if (state == NULL) return; // Check for null pointer
    float dt = (now - last_time) * 1e-3f; // Calculate time difference in seconds
    last_time = now; // Update last time
    if (dt <= 0.0f) return; // Avoid division by zero
    // Calculate the average wheel speed
    state->speed = d.speed;
    state->pose.theta = sumTheta(d.yaw, -state->pose.initial_theta); // Update theta with current yaw
    state->pose.x += state->speed.linear_velocity * dt * cos(state->pose.theta);
    state->pose.y += state->speed.linear_velocity * dt * sin(state->pose.theta);
    //目前没有发现其他数据的作用，暂且处理这些数据
}


WheelSpeed SpeedToWheelSpeed(Speed speed)
{
    WheelSpeed wheel_speed;
    // Assuming a differential drive robot, calculate left and right wheel speeds
    wheel_speed.left_wheel_speed = speed.linear_velocity - (speed.angular_velocity / 180 * PI * WHEEL_DIS / 2000);
    wheel_speed.right_wheel_speed = speed.linear_velocity + (speed.angular_velocity / 180 * PI * WHEEL_DIS / 2000);
    return wheel_speed;
}
float sumTheta(float theta1, float theta2)
{
    float sum = theta1 + theta2;
    if (sum < -180.0f) {
        sum += 360.0f; // Normalize angle to [-180, 180]
    } else if (sum > 180.0f) {
        sum -= 360.0f; // Normalize angle to [-180, 180]
    }
    return sum;
}


Speed PID_Move(float v, float w, float dt, short isreload)
{
    static PIDdata pidSpeed;   // PID控制直线速度
    static PIDdata pidAngular; // PID控制角速度
    static Speed last_speed; // 上一次的速度

    Speed final_speed = {0.0f, 0.0f}; // 初始化最终速度
    // PID参数，可根据实际情况调整
    float K_p_v = 1.0f;
    float K_i_v = 0.0f;
    float K_d_v = 0.0f;

    float K_p_w = 1.0f;
    float K_i_w = 0.1f;
    float K_d_w = 0.1f;

    last_speed.linear_velocity = v; // 重置上一次的线速度
    last_speed.angular_velocity = w; // 重置上一次的角速度

    if (isreload) {
        PID_Init(&pidSpeed);
        PID_Init(&pidAngular);
    }
    else{
        Data data = getData(); // 获取当前速度和角速度
        final_speed = data.speed; // 获取当前速度

        PID_Update(&pidSpeed, last_speed.linear_velocity, data.speed.linear_velocity, dt);
        PID_Update(&pidAngular, last_speed.angular_velocity, data.speed.angular_velocity, dt);
    }
    Speed target_speed;
    target_speed.linear_velocity = v + PID_Compute(&pidSpeed, K_p_v, K_i_v, K_d_v);
    target_speed.angular_velocity = w + PID_Compute(&pidAngular, K_p_w, K_i_w, K_d_w);

    WheelSpeed wheel_speed = SpeedToWheelSpeed(target_speed);

    LSet((int16_t)(STOD * wheel_speed.left_wheel_speed)); // 设置左轮速度
    RSet((int16_t)(STOD * wheel_speed.right_wheel_speed)); // 设置右轮速度
    
    return final_speed; // 返回初始速度
}
float runCircle(float radius, float speed, float angle, DIR dir)
{
    static int first_run = 1; // 静态变量用于判断是否第一次运行
    static uint32_t last_time = 0; // 上次更新时间
    static float target_angle = 0.0f; // 目标角度
    uint32_t now = HAL_GetTick(); // 获取当前时间

    float linear_velocity = speed; // 线速度
    float angular_velocity = linear_velocity / radius * (180 / PI); // 角速度，转换为度

    if (first_run || now - last_time > 1000 ) // 如果是第一次运行或超过1秒未更新
    {
        first_run = 0; // 标记为非第一次运行
        last_time = now; // 更新上次更新时间
        target_angle = angle; // 设置目标角度
        PID_Move(linear_velocity, dir == LEFT? angular_velocity : -angular_velocity, 0.1f, 1); // 初始化PID控制
        return target_angle;
    }

    float dt = (now - last_time) / 1000.0f; // 计算时间间隔
    last_time = now; // 更新上次更新时间
    Speed current_speed = PID_Move(linear_velocity, dir == LEFT? angular_velocity : -angular_velocity, dt, 0); // 更新速度
    target_angle -= fabs(current_speed.angular_velocity) * dt * 8 ; // 更新目标角度
    return target_angle; // 返回剩余角度
}
float Straight(float distance, float speed, float yaw, DIR dir)
{
    static int first_run = 1; // 静态变量用于判断是否第一次运行
    static uint32_t last_time = 0; // 上次更新时间
    static float target_distance = 0.0f; // 目标距离
    uint32_t now = HAL_GetTick(); // 获取当前时间
    Data data = getData(); // 获取当前速度和角速度
    float k_w = 6; // 角速度的系数

    speed = speed * (dir == FORWARD ? 1 : -1); // 根据方向调整速度

    if (first_run || now - last_time > 1000 ) // 如果是第一次运行或超过1秒未更新
    {
        first_run = 0; // 标记为非第一次运行
        last_time = now; // 更新上次更新时间
        target_distance = distance; // 设置目标距离
        PID_Move(speed, -k_w * sumTheta(data.yaw, -yaw), 0.1f, 1); // 初始化PID控制
        return target_distance;
    }
    float dt = (now - last_time) / 1000.0f; // 计算时间间隔
    last_time = now; // 更新上次更新时间
    Speed current_speed = PID_Move(speed, -k_w * sumTheta(data.yaw, -yaw), dt, 0); // 更新速度
    target_distance -= fabs(current_speed.linear_velocity) * dt; // 更新目标距离
    return target_distance; // 返回剩余距离
    
}

Data getData()
{
    Data data;
    data.speed.angular_velocity = getWz(); // 获取当前的角速度
    data.speed.linear_velocity = cSpeed();
    data.yaw = getYaw(); // 获取当前的偏航角

    return data;
}
