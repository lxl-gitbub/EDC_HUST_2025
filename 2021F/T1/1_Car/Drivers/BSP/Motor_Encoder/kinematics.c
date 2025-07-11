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
    if (state == NULL) return; // Check for null pointer and valid dt
    // Calculate the average wheel speed
    state->speed = d.speed;
    state->pose.theta = sumTheta(d.yaw, -state->pose.initial_theta); // Update theta with current yaw
    //目前没有发现其他数据的作用，暂且处理这些数据
}


WheelSpeed SpeedToWheelSpeed(Speed speed)
{
    WheelSpeed wheel_speed;
    // Assuming a differential drive robot, calculate left and right wheel speeds
    wheel_speed.left_wheel_speed = speed.linear_velocity - (speed.angular_velocity * WHEEL_DIAMETER / 2.0f);
    wheel_speed.right_wheel_speed = speed.linear_velocity + (speed.angular_velocity * WHEEL_DIAMETER / 2.0f);
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

//这个函数的逻辑已经实现了，但是参数是不完善的，且dis的计算是不精准的，是有待优化的
float Straight(float dis, float speed)//单位为米和米每秒
{
    static float target_dis = 0;
    MOVETYPE type = dis < 0 ? BACK : FOR;
    dis = fabs(dis);
		
    static short first_run = 1; // 静态变量用于判断是否第一次运行
    static float dist_e = 0.0f; // 静态变量用于记录已移动的距离
    static uint32_t last_time = 0; // 上次更新时间
    static PIDdata pidSpeed; // PID控制直线速度
    static PIDdata pidAngular; // PID控制角速度
    static float yaw = 0.0f; // 偏航角

    float K_p_v = 1000.0f; // 比例系数
    float K_i_v = 0.0f; // 积分系数
    float K_d_v = 0.0f; // 微分系数

    float K_p_w = 0.1f; // 比例系数
    float K_i_w = 0.0f; // 积分系数
    float K_d_w = 0.0f; // 微分系数

    
    uint32_t now = HAL_GetTick(); // 获取当前时间
    if (first_run || fabs(dis - target_dis) > 0.01f)// 如果是第一次运行或超过2秒未更新, 将该函数初始化
    {
        first_run = 0; // 标记为非第一次运行
        target_dis = dis;
        dist_e = dis; // 重置距离
        last_time = now; // 更新上次更新时间
        PID_Init(&pidSpeed); 
        PID_Init(&pidAngular);// 初始化PID数据
        yaw = getYaw(); // 获取当前偏航角
        return dis; // 表示未开始移动
    }

    //处理时间
    float dt = (now - last_time) / 1000.0f; // 计算时间间隔
    last_time = now;

    Data data = getData(); // 获取当前数据
    if(type == BACK) // 如果是后退
    {
        data.speed.linear_velocity = -data.speed.linear_velocity; // 反转线速度
        data.yaw = -data.yaw; // 反转偏航角
    }
    PID_Update(&pidSpeed, speed, data.speed.linear_velocity, dt); // 更新PID数据
    PID_Update(&pidAngular, 0, data.speed.angular_velocity, dt);

    Speed target_speed;
    target_speed.linear_velocity = PID_Compute(&pidSpeed, K_p_v, K_i_v, K_d_v); // 计算目标线速度
    
    target_speed.angular_velocity = PID_Compute(&pidAngular, K_p_w, K_i_w, K_d_w); // 计算目标角速度

    WheelSpeed wheel_speed = SpeedToWheelSpeed(target_speed); // 将速度转换为轮速
    wheel_speed.left_wheel_speed = (wheel_speed.left_wheel_speed > 0) ? wheel_speed.left_wheel_speed : 0; // 确保左轮速度不小于0
    wheel_speed.right_wheel_speed = (wheel_speed.right_wheel_speed > 0) ? wheel_speed.right_wheel_speed : 0; // 确保右轮速度不小于0
    wheel_speed.left_wheel_speed = (wheel_speed.left_wheel_speed < 1000) ? wheel_speed.left_wheel_speed : 1000; // 限制左轮速度最大值
    wheel_speed.right_wheel_speed = (wheel_speed.right_wheel_speed < 1000) ? wheel_speed.right_wheel_speed : 1000; // 限制右轮速度最大值

    LMotorSet(type, (uint16_t) wheel_speed.left_wheel_speed); // 设置左轮速度
    RMotorSet(type, (uint16_t) wheel_speed.right_wheel_speed); // 设置右轮速度
		
    dist_e -= data.speed.linear_velocity * dt; // 累加已移动的距离
    return dist_e; // 返回已移动的距离
}

//简陋的左转函数，需要优化，将来可能会改为PID控制
//假设左转角度为angle，单位为角度
void TurnLeft(float angle)
{
    LMotorSet(BACK, 100); // 左轮前进
    RMotorSet(FOR, 100); // 右轮后退
    float targetYaw = sumTheta(getYaw(), angle); // 计算目标角度
    while(fabs(getYaw() - targetYaw) > 3) // 当偏航角与目标角度的差值大于0.01时
    {
        HAL_Delay(10); // 等待10毫秒
    } 
    LMotorSet(BREAK, 0); // 左轮停止
    RMotorSet(BREAK, 0); // 右轮停止
}


Data getData()
{
    Data data;
    data.speed.angular_velocity = getWz(); // 获取当前的角速度
    data.speed.linear_velocity = cSpeed();
    data.yaw = getYaw(); // 获取当前的偏航角

    return data;
}
