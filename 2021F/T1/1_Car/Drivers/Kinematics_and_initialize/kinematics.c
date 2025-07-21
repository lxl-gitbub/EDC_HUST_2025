#include "kinematics.h"
#include "AllHeader.h"
#include "Initialize.h"

// --- PID�ṹ������غ������ֲ��� ---

void PID_Init(PIDdata *pid) {
    if (pid == NULL) return;
    pid->error = 0.0f;
    pid->sum = 0.0f;
    pid->difference = 0.0f;
}
void PID_Update(PIDdata *pid, float target, float current, float dt) {
    if (pid == NULL || dt <= 0.0f) return;
    float preerror = pid->error;
    pid->error = target - current;
    pid->sum += pid->error * dt;
    pid->difference = (pid->error - preerror) / dt;
}
float PID_Compute(PIDdata *pid, float Kp, float Ki, float Kd) {
    if (pid == NULL) return 0.0f;
    if (pid->sum > 1000.0f) {
        pid->sum = 1000.0f; // ��ֹ���ֹ���
    } else if (pid->sum < -1000.0f) {
        pid->sum = -1000.0f; // ��ֹ���ֹ�С
    }
    return Kp * pid->error + Ki * pid->sum + Kd * pid->difference;
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
    // Calculate the average wheel speed
    state->speed = d.speed;
    state->pose.theta = sumTheta(d.yaw, -state->pose.initial_theta); // Update theta with current yaw
    state->pose.x += state->speed.linear_velocity * d.dt * cos(state->pose.theta);
    state->pose.y += state->speed.linear_velocity * d.dt * sin(state->pose.theta);
    //Ŀǰû�з����������ݵ����ã����Ҵ�����Щ����
}


// --- �Ƕȹ�һ�� ---
float sumTheta(float theta1, float theta2)
{
    float sum = theta1 + theta2;
    while (sum < -180.0f) sum += 360.0f;
    while (sum > 180.0f) sum -= 360.0f;
    return sum;
}

// --- ����ת�� ---
WheelSpeed SpeedToWheelSpeed(Speed speed)
{
    WheelSpeed wheel_speed;
    wheel_speed.left_wheel_speed = speed.linear_velocity - (speed.angular_velocity / 180.0f * PI * WHEEL_DIS / 2000.0f);
    wheel_speed.right_wheel_speed = speed.linear_velocity + (speed.angular_velocity / 180.0f * PI * WHEEL_DIS / 2000.0f);
    return wheel_speed;
}

// --- PID�ٶȿ��� ---
// ���ɣ�ֱ����current_data��dt��current_data.dt����ѭ����֤UpdateData()�ѵ���
Speed PID_Move(float v, float w, short isreload)
{
    static PIDdata pidSpeed;
    static PIDdata pidAngular;
    static Speed last_target = {0.0f, 0.0f};

    // PID�����ɸ���ʵ�ʵ���
    float K_p_v = 1.0f, K_i_v = 0.0f, K_d_v = 0.0f;
    float K_p_w = 1.0f, K_i_w = 0.1f, K_d_w = 0.1f;

    if (isreload) {
        PID_Init(&pidSpeed);
        PID_Init(&pidAngular);
        last_target.linear_velocity = v;
        last_target.angular_velocity = w;
        return current_data.speed;
    }

    // ��current_data.dt��Ϊdt
    float dt = current_data.dt;
    if (dt <= 0.0f) dt = 0.01f; // ��ֹdtΪ0

    PID_Update(&pidSpeed, v, current_data.speed.linear_velocity, dt);
    PID_Update(&pidAngular, w, current_data.speed.angular_velocity, dt);

    Speed target_speed;
    target_speed.linear_velocity = v + PID_Compute(&pidSpeed, K_p_v, K_i_v, K_d_v);
    target_speed.angular_velocity = w + PID_Compute(&pidAngular, K_p_w, K_i_w, K_d_w);

    WheelSpeed wheel_speed = SpeedToWheelSpeed(target_speed);

    LSet((int16_t)(STOD * wheel_speed.left_wheel_speed));
    RSet((int16_t)(STOD * wheel_speed.right_wheel_speed));

    return current_data.speed;
}

// --- ֱ�п��� ---
// ���ɣ���current_data��dt��current_data.dt��yaw�����sumTheta(current_data.yaw, -target_yaw)
bool Straight(float distance, float speed, float target_yaw, DIR dir)
{
    static int first_run = 1;
    static float remain = 0.0f;
    static float total = 0.0f;
		static uint32_t last_time = 0;
		uint32_t now = HAL_GetTick();

    if (first_run && now - last_time > 1000) {
        first_run = 0;
        remain = distance;
        total = distance;
        PID_Move(0, 0, 1); // ��λPID
        return false;
    }
		last_time = now;

    float dt = current_data.dt;
		
    float v = (dir == FORWARD ? speed : -speed);
    float yaw_error = sumTheta(current_data.yaw, -target_yaw);
    float k_w = 6.0f; // �Ƕ�����ϵ��

    Speed cur = PID_Move(v, -k_w * yaw_error, 0);
    remain -= fabs(cur.linear_velocity) * dt;

    // ����Ŀ������λfirst_run
//    if (fabs(remain) < 0.02f) {
//        first_run = 1;
//        LSet(0);
//        RSet(0);
//        return true;
//    }
    return false;
}

// --- Բ���˶� ---
// ���ɣ���current_data��dt��current_data.dt���ǶȻ�����ʵ�ʽ��ٶ�
bool runCircle(float radius, float speed, float angle, DIR dir)
{
    static int first_run = 1;
    static float start_yaw = 0.0f;
		static uint32_t last_time = 0;
		uint32_t now = HAL_GetTick();

    if (first_run || now - last_time > 1000) {
        first_run = 0;
        start_yaw = current_data.yaw;
        PID_Move(0, 0, 1); // ��λPID
				last_time = now;
        RED_up();
        return false;
    }
	last_time = now;
    float linear_velocity = speed;
    float angular_velocity = linear_velocity / radius * (180.0f / PI);
    if (dir == RIGHT) angular_velocity = -angular_velocity;

    Speed cur = PID_Move(linear_velocity, angular_velocity, 0);

    // ��ʵ��yaw����ʼyaw�Ĳ�ֵ�ж�ʣ��Ƕ�
    float delta_yaw = sumTheta(current_data.yaw, -start_yaw);
    float remain_angle = angle - fabs(delta_yaw);

    // ����Ŀ��ǶȺ�λfirst_run
//    if (fabs(remain_angle) < 2.0f) {
//        first_run = 1;
//        LSet(0);
//        RSet(0);
//        return true;
//    }
    return false;
}
