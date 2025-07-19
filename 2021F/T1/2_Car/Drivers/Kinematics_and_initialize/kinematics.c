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
    //Ŀǰû�з����������ݵ����ã����Ҵ�����Щ����
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
    static PIDdata pidSpeed;   // PID����ֱ���ٶ�
    static PIDdata pidAngular; // PID���ƽ��ٶ�
    static Speed last_speed; // ��һ�ε��ٶ�

    Speed final_speed = {0.0f, 0.0f}; // ��ʼ�������ٶ�
    // PID�������ɸ���ʵ���������
    float K_p_v = 1.0f;
    float K_i_v = 0.0f;
    float K_d_v = 0.0f;

    float K_p_w = 1.0f;
    float K_i_w = 0.1f;
    float K_d_w = 0.1f;

    last_speed.linear_velocity = v; // ������һ�ε����ٶ�
    last_speed.angular_velocity = w; // ������һ�εĽ��ٶ�

    if (isreload) {
        PID_Init(&pidSpeed);
        PID_Init(&pidAngular);
    }
    else{
        Data data = getData(); // ��ȡ��ǰ�ٶȺͽ��ٶ�
        final_speed = data.speed; // ��ȡ��ǰ�ٶ�

        PID_Update(&pidSpeed, last_speed.linear_velocity, data.speed.linear_velocity, dt);
        PID_Update(&pidAngular, last_speed.angular_velocity, data.speed.angular_velocity, dt);
    }
    Speed target_speed;
    target_speed.linear_velocity = v + PID_Compute(&pidSpeed, K_p_v, K_i_v, K_d_v);
    target_speed.angular_velocity = w + PID_Compute(&pidAngular, K_p_w, K_i_w, K_d_w);

    WheelSpeed wheel_speed = SpeedToWheelSpeed(target_speed);

    LSet((int16_t)(STOD * wheel_speed.left_wheel_speed)); // ���������ٶ�
    RSet((int16_t)(STOD * wheel_speed.right_wheel_speed)); // ���������ٶ�
    
    return final_speed; // ���س�ʼ�ٶ�
}
float runCircle(float radius, float speed, float angle, DIR dir)
{
    static int first_run = 1; // ��̬���������ж��Ƿ��һ������
    static uint32_t last_time = 0; // �ϴθ���ʱ��
    static float target_angle = 0.0f; // Ŀ��Ƕ�
    uint32_t now = HAL_GetTick(); // ��ȡ��ǰʱ��

    float linear_velocity = speed; // ���ٶ�
    float angular_velocity = linear_velocity / radius * (180 / PI); // ���ٶȣ�ת��Ϊ��

    if (first_run || now - last_time > 1000 ) // ����ǵ�һ�����л򳬹�1��δ����
    {
        first_run = 0; // ���Ϊ�ǵ�һ������
        last_time = now; // �����ϴθ���ʱ��
        target_angle = angle; // ����Ŀ��Ƕ�
        PID_Move(linear_velocity, dir == LEFT? angular_velocity : -angular_velocity, 0.1f, 1); // ��ʼ��PID����
        return target_angle;
    }

    float dt = (now - last_time) / 1000.0f; // ����ʱ����
    last_time = now; // �����ϴθ���ʱ��
    Speed current_speed = PID_Move(linear_velocity, dir == LEFT? angular_velocity : -angular_velocity, dt, 0); // �����ٶ�
    target_angle -= fabs(current_speed.angular_velocity) * dt * 8 ; // ����Ŀ��Ƕ�
    return target_angle; // ����ʣ��Ƕ�
}
float Straight(float distance, float speed, float yaw, DIR dir)
{
    static int first_run = 1; // ��̬���������ж��Ƿ��һ������
    static uint32_t last_time = 0; // �ϴθ���ʱ��
    static float target_distance = 0.0f; // Ŀ�����
    uint32_t now = HAL_GetTick(); // ��ȡ��ǰʱ��
    Data data = getData(); // ��ȡ��ǰ�ٶȺͽ��ٶ�
    float k_w = 6; // ���ٶȵ�ϵ��

    speed = speed * (dir == FORWARD ? 1 : -1); // ���ݷ�������ٶ�

    if (first_run || now - last_time > 1000 ) // ����ǵ�һ�����л򳬹�1��δ����
    {
        first_run = 0; // ���Ϊ�ǵ�һ������
        last_time = now; // �����ϴθ���ʱ��
        target_distance = distance; // ����Ŀ�����
        PID_Move(speed, -k_w * sumTheta(data.yaw, -yaw), 0.1f, 1); // ��ʼ��PID����
        return target_distance;
    }
    float dt = (now - last_time) / 1000.0f; // ����ʱ����
    last_time = now; // �����ϴθ���ʱ��
    Speed current_speed = PID_Move(speed, -k_w * sumTheta(data.yaw, -yaw), dt, 0); // �����ٶ�
    target_distance -= fabs(current_speed.linear_velocity) * dt; // ����Ŀ�����
    return target_distance; // ����ʣ�����
    
}

Data getData()
{
    Data data;
    data.speed.angular_velocity = getWz(); // ��ȡ��ǰ�Ľ��ٶ�
    data.speed.linear_velocity = cSpeed();
    data.yaw = getYaw(); // ��ȡ��ǰ��ƫ����

    return data;
}
