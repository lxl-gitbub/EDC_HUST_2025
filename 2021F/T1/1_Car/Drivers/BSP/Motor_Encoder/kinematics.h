#ifndef KINEMATICS_H
#define KINEMATICS_H

#include "AllHeader.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct{
    float error; // 误差
    float sum; // 积分
    float difference; // 微分
}PIDdata;

// 运动学函数声明