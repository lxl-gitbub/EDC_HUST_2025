#include "Cont_SMotor.h"
#include <math.h>
#include <stdbool.h>

Coordinate laser_position = {0.0f, 0.0f}; // 激光雷达位置
Coordinate target_position = {0.0f, 0.0f}; // 目标位置
bool is_updated = false; // 是否更新了数据
bool is_new_mode = false; // 是否进入了新模式

Attitude CoordinateToAttitude(Coordinate coord) {
    Attitude attitude;
    // 计算方向角和俯仰角
    attitude.yaw = -atan2f(XDIS_MOTOR_O + coord.x, ZDIS_MOTOR_O) * 180.0f / C_PI; // 转换为角度
    attitude.pitch = atan2f(YDIS_MOTOR_O + coord.y, sqrtf(powf(XDIS_MOTOR_O + coord.x, 2) + powf(ZDIS_MOTOR_O, 2))) * 180.0f / C_PI;
    return attitude;
}

void PID_SMotor_Cont(void)
{
    static PIDdata pid_yaw, pid_pitch;
    static uint32_t last_update_time = 0;
    uint32_t current_time = HAL_GetTick();
    if(is_new_mode) {
        PID_Init(&pid_yaw);
        PID_Init(&pid_pitch);
        is_new_mode = false; // 重置新模式标志
        last_update_time = current_time; // 更新上次更新时间
        return; // 如果是新模式则不进行PID计算
    }
    if(!is_updated) return; // 如果数据未更新则返回
    is_updated = false; // 重置更新标志
    Attitude target_attitude = CoordinateToAttitude(target_position);
    Attitude attitude = CoordinateToAttitude(laser_position);
    PID_Update(&pid_yaw, target_attitude.yaw, attitude.yaw, current_time - last_update_time);
    PID_Update(&pid_pitch, target_attitude.pitch, attitude.pitch, current_time - last_update_time);
    last_update_time = current_time; // 更新上次更新时间
    // 计算PID输出
    float output_yaw = PID_Compute(&pid_yaw, 1.0f, 0.1f, 0.01f);
    float output_pitch = PID_Compute(&pid_pitch, 1.0f, 0.1f, 0.01f);
    // 发送控制命令
    YP_SMotor_SetSpeed(output_yaw, output_pitch); // 设置舵机速度
    YP_SMotor_UpdateState(); // 更新舵机状态
}