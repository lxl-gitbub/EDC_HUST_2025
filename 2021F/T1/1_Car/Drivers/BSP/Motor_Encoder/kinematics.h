#ifndef KINEMATICS_H
#define KINEMATICS_H

#include "AllHeader.h"

#define STOD 999/1.30

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    L = 0, // ��ת
    R = 1, // ��ת
}Direction;

typedef struct{
    float error; // ���
    float sum; // ����
    float difference; // ΢��
}PIDdata;

typedef struct{
    float Kp; // ����ϵ��
    float Ki; // ����ϵ��
    float Kd; // ΢��ϵ��
}PIDConfig;

typedef struct{
    float x; // x��λ��
    float y; // y��λ��
    float theta; // �����
    float initial_theta; // ��ʼ�����
}Pose;

typedef struct{
    float linear_velocity; // ���ٶ�
    float angular_velocity; // ���ٶ�
}Speed;

typedef struct{
    float left_wheel_speed; // �����ٶ�
    float right_wheel_speed; // �����ٶ�
}WheelSpeed;

typedef struct{
    Speed speed; // ���ٶȺͽ��ٶ�
    float yaw; // ƫ����
}Data; //������ݽṹ���ڴӱ��������������л�ȡ����

typedef struct{
    Pose pose; // λ�úͷ���
    Speed speed; // ���ٶȺͽ��ٶ�
    WheelSpeed wheel_speed; // �������ٶ�
}CarState;

typedef struct{
    CarState car_state; // ����״̬
    PIDdata pid; // PID����
}CarKinematics;

#ifdef __cplusplus
extern "C" {
#endif

//PID ��������
void PID_Init(PIDdata *pid);
void PID_Update(PIDdata *pid, float target, float current, float dt);
float PID_Compute(PIDdata *pid, float Kp, float Ki, float Kd);

//С��״̬����������
void CarState_Init(CarState *state);
void CarState_Update(CarState *state,  Data d);

// С���ٶȻ��㺯��
WheelSpeed SpeedToWheelSpeed(Speed speed);
float sumTheta(float, float);

// ��ȡ���ݺ�������
Data getData();

// �˶�ѧ��������
float Straight(float dis, float speed);
void TurnLeft(float angle); // ��ת����
void TurnRight(float angle); // ��ת����
void Turn90(short dir); // ת����
void ForCar(); // ǰ��һ������

Speed PID_Move(float v, float w, float dt, short isreload);
float runCircle(float radius, float speed, float angle, Direction dir); // Բ���˶�����

 #endif