#ifndef __LASER_H
#define __LASER_H

#include "usart.h"		// ������
#include "string.h"		// �ַ���/���鴦��
#include <stdint.h>		// uint����ʹ��

// ���崮�ڽ��ܳ���
#define USART_LASER_RX_BUF_LEN 25

// ���弤��ģʽ���ں�
#define HUART_LASER huart1
#define USART_LASER USART1

// ��������������������
void Laser_Ask_for_Loc () ;
	
// �õ�������Ļλ��
int Laser_Get_Loc(uint8_t USART_RX_BUF[] ) ;

void Laser_Mode( uint8_t USART_RX_BUF[] ,UART_HandleTypeDef * huart) ;

#endif
