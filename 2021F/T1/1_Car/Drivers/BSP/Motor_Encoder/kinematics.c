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
    //Ŀǰû�з����������ݵ����ã����Ҵ�����Щ����
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

//����������߼��Ѿ�ʵ���ˣ����ǲ����ǲ����Ƶģ���dis�ļ����ǲ���׼�ģ����д��Ż���
float Straight(float dis, float speed)//��λΪ�׺���ÿ��
{
    static float target_dis = 0;
    MOVETYPE type = dis < 0 ? BACK : FOR;
    dis = fabs(dis);
		
    static short first_run = 1; // ��̬���������ж��Ƿ��һ������
    static float dist_e = 0.0f; // ��̬�������ڼ�¼���ƶ��ľ���
    static uint32_t last_time = 0; // �ϴθ���ʱ��
    static PIDdata pidSpeed; // PID����ֱ���ٶ�
    static PIDdata pidAngular; // PID���ƽ��ٶ�
    static float yaw = 0.0f; // ƫ����

    float K_p_v = 1000.0f; // ����ϵ��
    float K_i_v = 0.0f; // ����ϵ��
    float K_d_v = 0.0f; // ΢��ϵ��

    float K_p_w = 0.1f; // ����ϵ��
    float K_i_w = 0.0f; // ����ϵ��
    float K_d_w = 0.0f; // ΢��ϵ��

    
    uint32_t now = HAL_GetTick(); // ��ȡ��ǰʱ��
    if (first_run || fabs(dis - target_dis) > 0.01f)// ����ǵ�һ�����л򳬹�2��δ����, ���ú�����ʼ��
    {
        first_run = 0; // ���Ϊ�ǵ�һ������
        target_dis = dis;
        dist_e = dis; // ���þ���
        last_time = now; // �����ϴθ���ʱ��
        PID_Init(&pidSpeed); 
        PID_Init(&pidAngular);// ��ʼ��PID����
        yaw = getYaw(); // ��ȡ��ǰƫ����
        return dis; // ��ʾδ��ʼ�ƶ�
    }

    //����ʱ��
    float dt = (now - last_time) / 1000.0f; // ����ʱ����
    last_time = now;

    Data data = getData(); // ��ȡ��ǰ����
    if(type == BACK) // ����Ǻ���
    {
        data.speed.linear_velocity = -data.speed.linear_velocity; // ��ת���ٶ�
        data.yaw = -data.yaw; // ��תƫ����
    }
    PID_Update(&pidSpeed, speed, data.speed.linear_velocity, dt); // ����PID����
    PID_Update(&pidAngular, 0, data.speed.angular_velocity, dt);

    Speed target_speed;
    target_speed.linear_velocity = PID_Compute(&pidSpeed, K_p_v, K_i_v, K_d_v); // ����Ŀ�����ٶ�
    
    target_speed.angular_velocity = PID_Compute(&pidAngular, K_p_w, K_i_w, K_d_w); // ����Ŀ����ٶ�

    WheelSpeed wheel_speed = SpeedToWheelSpeed(target_speed); // ���ٶ�ת��Ϊ����
    wheel_speed.left_wheel_speed = (wheel_speed.left_wheel_speed > 0) ? wheel_speed.left_wheel_speed : 0; // ȷ�������ٶȲ�С��0
    wheel_speed.right_wheel_speed = (wheel_speed.right_wheel_speed > 0) ? wheel_speed.right_wheel_speed : 0; // ȷ�������ٶȲ�С��0
    wheel_speed.left_wheel_speed = (wheel_speed.left_wheel_speed < 1000) ? wheel_speed.left_wheel_speed : 1000; // ���������ٶ����ֵ
    wheel_speed.right_wheel_speed = (wheel_speed.right_wheel_speed < 1000) ? wheel_speed.right_wheel_speed : 1000; // ���������ٶ����ֵ

    LMotorSet(type, (uint16_t) wheel_speed.left_wheel_speed); // ���������ٶ�
    RMotorSet(type, (uint16_t) wheel_speed.right_wheel_speed); // ���������ٶ�
		
    dist_e -= data.speed.linear_velocity * dt; // �ۼ����ƶ��ľ���
    return dist_e; // �������ƶ��ľ���
}

//��ª����ת��������Ҫ�Ż����������ܻ��ΪPID����
//������ת�Ƕ�Ϊangle����λΪ�Ƕ�
void TurnLeft(float angle)
{
    LMotorSet(BACK, 100); // ����ǰ��
    RMotorSet(FOR, 100); // ���ֺ���
    float targetYaw = sumTheta(getYaw(), angle); // ����Ŀ��Ƕ�
    while(fabs(getYaw() - targetYaw) > 3) // ��ƫ������Ŀ��ǶȵĲ�ֵ����0.01ʱ
    {
        HAL_Delay(10); // �ȴ�10����
    } 
    LMotorSet(BREAK, 0); // ����ֹͣ
    RMotorSet(BREAK, 0); // ����ֹͣ
}


Data getData()
{
    Data data;
    data.speed.angular_velocity = getWz(); // ��ȡ��ǰ�Ľ��ٶ�
    data.speed.linear_velocity = cSpeed();
    data.yaw = getYaw(); // ��ȡ��ǰ��ƫ����

    return data;
}
