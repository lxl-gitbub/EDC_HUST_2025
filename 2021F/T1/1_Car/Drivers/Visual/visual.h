#ifndef VISUAL_H
#define VISUAL_H

#include "AllHeader.h"

// ������ܳ���
#define USART_RX_BUF_LEN 25 
// ����
#define MAX_SAMPLES 10
// ������ܳ���
#define USART_EYE_RX_LEN 25 

void setRoomArray(void);
void SampleGoalNumFromArr(uint8_t arr[]);
void ResetSampling();
void control(void);
// ���ƺ���
// ���ݲ����������ģʽ�ķ���
#endif // VISUAL_H