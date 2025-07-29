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

// JY61P ������ͷ�ļ�
#include "REG.h"
#include "wit_c_sdk.h"

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

// ����ͱ�������ʼ��ͷ�ļ����ڲ��������˶�����ͷ�ļ�
#include "Initialize.h"

//pid��������ͷ�ļ�
#include "pid.h"

//OLED��ʾ��ͷ�ļ�
#include "oled.h"

//�Ӿ�ͨ��ͷ�ļ�
#include "visual.h"

// ledͷ�ļ�
#include "led.h"

// ��������Ϣ����ͷ�ļ�
#include "SensorProc.h"
#include "Rotation.h"

#ifdef __cplusplus
extern "C" {
#endif

#endif /* ALLHEADER_H */