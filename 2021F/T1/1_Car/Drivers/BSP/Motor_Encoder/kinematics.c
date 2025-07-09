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

WheelSpeed SpeedToWheelSpeed(Speed speed)
{
    WheelSpeed wheel_speed;
    // Assuming a differential drive robot, calculate left and right wheel speeds
    wheel_speed.left_wheel_speed = speed.linear_velocity - (speed.angular_velocity * WHEEL_DIAMETER / 2.0f);
    wheel_speed.right_wheel_speed = speed.linear_velocity + (speed.angular_velocity * WHEEL_DIAMETER / 2.0f);
    return wheel_speed;
}

float Straight(float dis, float speed)
{
    MOVETYPE type = FOR;
    if(dis < 0) {
        type = BACK; // 如果距离为负，则向后移动
        dis = -dis; // 取绝对值
    }

    static short first_run = 1; // 静态变量用于判断是否第一次运行
    static float dist_e = 0.0f; // 静态变量用于记录已移动的距离
    static uint32_t last_time = 0; // 上次更新时间
    static PIDdata pidSpeed; // PID控制直线速度
    static PIDdata pidAngular; // PID控制角速度
    static float yaw = 0.0f; // 偏航角

    float K_p = 1000.0f; // 比例系数
    float K_i = 0.0f; // 积分系数
    float K_d = 0.0f; // 微分系数

    
    uint32_t now = HAL_GetTick(); // 获取当前时间
    if (first_run || now - last_time >= 2000)// 如果是第一次运行或超过2秒未更新, 将该函数初始化
    {
        first_run = 0; // 标记为非第一次运行
        dist_e = dis; // 重置距离
        last_time = now; // 更新上次更新时间
        PID_Init(&pidSpeed); 
        PID_Init(&pidAngular);// 初始化PID数据
        yaw = getYaw(); // 获取当前偏航角
        return dis; // 表示未开始移动
    }
    Data data = getData(); // 获取当前数据
    PID_Update(&pidSpeed, speed, data.speed.linear_velocity, data.dt); // 更新PID数据
    PID_Update(&pidAngular, 0, data.speed.angular_velocity, data.dt);
    Speed target_speed;
    target_speed.linear_velocity = PID_Compute(&pidSpeed, K_p, K_i, K_d); // 计算目标线速度
    target_speed.angular_velocity = PID_Compute(&pidAngular, K_p, K_i, K_d); // 计算目标角速度
    WheelSpeed wheel_speed = SpeedToWheelSpeed(target_speed); // 将速度转换为轮速
    LMotorSet(type, (uint16_t) wheel_speed.left_wheel_speed); // 设置左轮速度
    RMotorSet(type, (uint16_t) wheel_speed.right_wheel_speed); // 设置右轮速度
    dist_e -= data.speed.linear_velocity * data.dt; // 累加已移动的距离
    return dist_e; // 返回已移动的距离
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
