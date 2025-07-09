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
        type = BACK; // �������Ϊ����������ƶ�
        dis = -dis; // ȡ����ֵ
    }

    static short first_run = 1; // ��̬���������ж��Ƿ��һ������
    static float dist_e = 0.0f; // ��̬�������ڼ�¼���ƶ��ľ���
    static uint32_t last_time = 0; // �ϴθ���ʱ��
    static PIDdata pidSpeed; // PID����ֱ���ٶ�
    static PIDdata pidAngular; // PID���ƽ��ٶ�
    static float yaw = 0.0f; // ƫ����

    float K_p = 1000.0f; // ����ϵ��
    float K_i = 0.0f; // ����ϵ��
    float K_d = 0.0f; // ΢��ϵ��

    
    uint32_t now = HAL_GetTick(); // ��ȡ��ǰʱ��
    if (first_run || now - last_time >= 2000)// ����ǵ�һ�����л򳬹�2��δ����, ���ú�����ʼ��
    {
        first_run = 0; // ���Ϊ�ǵ�һ������
        dist_e = dis; // ���þ���
        last_time = now; // �����ϴθ���ʱ��
        PID_Init(&pidSpeed); 
        PID_Init(&pidAngular);// ��ʼ��PID����
        yaw = getYaw(); // ��ȡ��ǰƫ����
        return dis; // ��ʾδ��ʼ�ƶ�
    }
    Data data = getData(); // ��ȡ��ǰ����
    PID_Update(&pidSpeed, speed, data.speed.linear_velocity, data.dt); // ����PID����
    PID_Update(&pidAngular, 0, data.speed.angular_velocity, data.dt);
    Speed target_speed;
    target_speed.linear_velocity = PID_Compute(&pidSpeed, K_p, K_i, K_d); // ����Ŀ�����ٶ�
    target_speed.angular_velocity = PID_Compute(&pidAngular, K_p, K_i, K_d); // ����Ŀ����ٶ�
    WheelSpeed wheel_speed = SpeedToWheelSpeed(target_speed); // ���ٶ�ת��Ϊ����
    LMotorSet(type, (uint16_t) wheel_speed.left_wheel_speed); // ���������ٶ�
    RMotorSet(type, (uint16_t) wheel_speed.right_wheel_speed); // ���������ٶ�
    dist_e -= data.speed.linear_velocity * data.dt; // �ۼ����ƶ��ľ���
    return dist_e; // �������ƶ��ľ���
}

Data getData()
{
    Data data;
    data.speed.angular_velocity = getWz(); // ��ȡ��ǰ�Ľ��ٶ�
    data.speed.linear_velocity = cSpeed();
    data.yaw = getYaw(); // ��ȡ��ǰ��ƫ����

    uint32_t now = HAL_GetTick();
    data.dt = now - time; // ����ʱ����
    time = now; // �����ϴλ�ȡ���ݵ�ʱ��

    return data;
}
