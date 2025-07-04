#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "stm32h7xx_hal.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

//陀螺仪头文件
#include "atk_ms601m.h"
#include "atk_ms601m_uart.h"

//八路巡线传感器头文件
#include "app_usart2.h"