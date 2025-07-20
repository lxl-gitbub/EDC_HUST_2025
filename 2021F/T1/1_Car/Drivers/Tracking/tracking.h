#ifndef _IRTRACKING_H_
#define _IRTRACKING_H_

#include "AllHeader.h"

void car_irtrack(void); //����PIDѲ��

void lineWalking_high(void); // ����Ѳ��
void lineWalking_low(void); // ����Ѳ��
void lineWalking_core(int16_t speed, float kp, float ki, float kd); // ����Ѳ���߼�������������
float PID_IR_Calc_Custom(int16_t actual_value, float kp, float ki, float
kd); // PID���㺯��������������
void Motion_Car_Control(int16_t V_x, int16_t V_y, int16_t V_z); // �˶����ƺ���

bool Road_detect(int, int);
bool half_Detect();
bool cross_Roads_Detect();

#endif
