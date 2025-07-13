#ifndef KINEMATICS_H
#define KINEMATICS_H

#include "AllHeader.h"

#define STOD 999/1.30

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    L = 0, // 左转
    R = 1, // 右转
}Direction;

typedef struct{
    float error; // 误差
    float sum; // 积分
    float difference; // 微分
}PIDdata;

typedef struct{
    float Kp; // 比例系数
    float Ki; // 积分系数
    float Kd; // 微分系数
}PIDConfig;

typedef struct{
    float x; // x轴位置
    float y; // y轴位置
    float theta; // 方向角
    float initial_theta; // 初始方向角
}Pose;

typedef struct{
    float linear_velocity; // 线速度
    float angular_velocity; // 角速度
}Speed;

typedef struct{
    float left_wheel_speed; // 左轮速度
    float right_wheel_speed; // 右轮速度
}WheelSpeed;

typedef struct{
    Speed speed; // 线速度和角速度
    float yaw; // 偏航角
}Data; //这个数据结构用于从编码器和陀螺仪中获取数据

typedef struct{
    Pose pose; // 位置和方向
    Speed speed; // 线速度和角速度
    WheelSpeed wheel_speed; // 左右轮速度
}CarState;

typedef struct{
    CarState car_state; // 汽车状态
    PIDdata pid; // PID数据
}CarKinematics;

#ifdef __cplusplus
extern "C" {
#endif

//PID 函数声明
void PID_Init(PIDdata *pid);
void PID_Update(PIDdata *pid, float target, float current, float dt);
float PID_Compute(PIDdata *pid, float Kp, float Ki, float Kd);

//小车状态处理函数声明
void CarState_Init(CarState *state);
void CarState_Update(CarState *state,  Data d);

// 小车速度换算函数
WheelSpeed SpeedToWheelSpeed(Speed speed);
float sumTheta(float, float);

// 获取数据函数声明
Data getData();

// 运动学函数声明
float Straight(float dis, float speed);
void TurnLeft(float angle); // 左转函数
void TurnRight(float angle); // 右转函数
void Turn90(short dir); // 转向函数
void ForCar(); // 前进一个车长

Speed PID_Move(float v, float w, float dt, short isreload);
float runCircle(float radius, float speed, float angle, Direction dir); // 圆周运动函数

 #endif