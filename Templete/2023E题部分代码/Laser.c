#include "RTE_Components.h"             // Component selection
#include "Laser.h"

/*
 -----------����ͨ�������������----------
 ���ڽ���:(�����ҵ�f4���Ӻ�CanMV)
 STM32_TX == PA9  ----- 12 == CanMV_RX(��)
 STM32_RX == PA10 ----- 11 == CanMV_TX(��)
*/


/* ------����-����ͨ����ز�����ʼ��------ */

// ����״̬�ͻ�����(���ڸô���ͨ�ŵĸ�����Ϣ����ȡ)
int USART_LASER_RX_BUF[USART_LASER_RX_BUF_LEN] ;

// ���������¼����
int Laser_Loc[USART_LASER_RX_BUF_LEN] ;

// �������
extern int error ;

/* ---------����-����ͨ����غ���--------- */

// ��������������������
void Laser_Ask_for_Loc ()
{
		if (HUART_LASER.gState == HAL_UART_STATE_READY) 
		{ 
			HAL_UART_Transmit_IT(&HUART_LASER, (uint8_t *)"L", 1); // ��������
		}
}

// �õ�������Ļλ��
int Laser_Get_Loc( uint8_t USART_LASER_RX_BUF[] )
{
		HAL_GPIO_TogglePin(LED0_GPIO_Port,  LED0_Pin) ;
		// ����λ�÷���֡: 0x12 target_x target_y real_x real_y 0x5B
		if ( USART_LASER_RX_BUF[0] == 0x12 && USART_LASER_RX_BUF[5] == 0x5B )
		{			
			// �õ������������
			for ( int j = 0 ; j < 4; j ++ )
			{
				Laser_Loc[j] = (int)USART_LASER_RX_BUF[j + 1] * 4 ; // �õ���ʵ����
			}
			return 1 ;
		}
		else
		{
			// �������
			return -1 ;
		}
}

// ��������ģʽ
void Laser_Mode( uint8_t USART_LASER_RX_BUF[] , UART_HandleTypeDef * huart)
{
	if (huart->Instance == USART_LASER)
	{
 		// ------------����ģʽ�ж�------------
			Laser_Ask_for_Loc() ;
		
			error = Laser_Get_Loc(USART_LASER_RX_BUF) ;
		
		// ��������ǰ���������
		memset(USART_LASER_RX_BUF, 0, USART_LASER_RX_BUF_LEN);
 
	  // �ٴν���
		HAL_UARTEx_ReceiveToIdle_IT(&HUART_LASER, USART_LASER_RX_BUF, USART_LASER_RX_BUF_LEN);
	}
}


