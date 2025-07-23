#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include "mode.h"//��Ҫ�õ���ص�mode����

#define BUF_LEN 256
#define UART_HANDLE &huart3// UART handle for Bluetooth communication

#ifdef __cplusplus
extern "C" {
#endif

void Bluetooth_Init(void);
void Bluetooth_Send(const char *data, size_t length);
void Bluetooth_Receive_Callback(UART_HandleTypeDef *huart, uint16_t Size);

#ifdef __cplusplus
}   // extern "C"
#endif

#endif // BLUETOOTH_H