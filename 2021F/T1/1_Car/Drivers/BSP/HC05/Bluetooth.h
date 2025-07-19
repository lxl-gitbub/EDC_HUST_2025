#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include "AllHeader.h"
#include "mode.h"//��Ҫ�õ���ص�mode����

#define BUF_LEN 256
#define UART_HANDLE &huart3// UART handle for Bluetooth communication

#ifdef __cplusplus
extern "C" {
#endif

void Bluetooth_Init(void);
void SendTrace(LOC loc);
void SendBegin(void);
void SendContinue(void);

#ifdef __cplusplus
}   // extern "C"
#endif

#endif // BLUETOOTH_H