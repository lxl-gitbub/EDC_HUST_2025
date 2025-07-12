#ifndef ALLHEADER_H
#define ALLHEADER_H

#include "main.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "stm32h7xx_hal.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// MS601M ������ͷ�ļ�
#include "atk_ms601m.h"
#include "atk_ms601m_uart.h"

// ��Ϊ�Ҷȴ�����ͷ�ļ�
#include "GanWei_i2c.h"
#include "gw_grayscale_sensor.h"

//Ѳ�߳���ͷ�ļ�
#include "tracking.h"
#include "tracking_delay.h"

// �������ͷ�ļ�
#include "tb6612fng.h"

// ����������ͷ�ļ�
#include "Hall_Encoder.h"

// �ڴ����ͷ�ļ�
#include "Mem.h"

// ����ͱ�������ʼ��ͷ�ļ�
#include "Initialize.h"

// �˶�ѧͷ�ļ�������������ֱ��ת�亯��
#include "kinematics.h"

#ifdef __cplusplus
extern "C" {
#endif

#endif /* ALLHEADER_H */