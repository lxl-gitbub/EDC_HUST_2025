#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "stm32h7xx_hal.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// MS601M 传感器头文件
#include "atk_ms601m.h"
#include "atk_ms601m_uart.h"

// 巡线应用头文件
#include "app_usart2.h"

// 电机驱动头文件
#include "tb6612fng.h"

// 霍尔编码器头文件
#include "Hall_Encoder.h"

// 内存管理头文件
#include "Mem.h"

// 电机和编码器初始化头文件
#include "Initialize.h"

// 运动学头文件，包括常见的直行转弯函数
#include "kinematics.h"