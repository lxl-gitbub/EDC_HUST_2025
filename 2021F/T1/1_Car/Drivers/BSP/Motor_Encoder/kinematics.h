#ifndef KINEMATICS_H
#define KINEMATICS_H

#include "AllHeader.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct{
    float error; // ���
    float sum; // ����
    float difference; // ΢��
}PIDdata;

// �˶�ѧ��������