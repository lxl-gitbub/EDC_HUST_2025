#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "stm32h7xx_hal.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// MS601M ������ͷ�ļ�
#include "atk_ms601m.h"
#include "atk_ms601m_uart.h"

// Ѳ��Ӧ��ͷ�ļ�
#include "app_usart2.h"

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