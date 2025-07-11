#include "bsp_motor_usart.h"

volatile bool  gConsoleTxDMATransmitted = false;
volatile bool  gConsoleTxTransmitted = true;

/************************************************
�������� �� Send_Motor_U8		Function name: Send_Motor_U8
��    �� �� USART1����һ���ַ�	Function: USART1 sends a character
��    �� �� Data --- ����		Parameter: Data --- data
�� �� ֵ �� ��					Return value: None
*************************************************/
void Send_Motor_U8(uint8_t Data)
{
	while( DL_UART_isBusy(UART_1_INST) == true );
	DL_UART_Main_transmitData(UART_1_INST, Data);
}

/************************************************
�������� �� Send_Motor_ArrayU8	Function name: Send_Motor_ArrayU8
��    �� �� ����1����N���ַ�		Function: Serial port 1 sends N characters
��    �� �� pData ---- �ַ���	Parameter: pData ---- string
            Length --- ����		Length --- length
�� �� ֵ �� ��					Return value: None
*************************************************/
void Send_Motor_ArrayU8(uint8_t *pData, uint16_t Length)
{
	while (Length--)
	{
		Send_Motor_U8(*pData);
		pData++;
	}
}

void UART_Console_write(uint8_t *data, uint16_t size)
{
    //�����ڷ�����Ϻ󣬲ſ��ٴη���   When the serial port has finished sending, it can only send again
    if(gConsoleTxTransmitted)
    {
        //����Դ��ַ Setting the source address
        DL_DMA_setSrcAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t)(data));
    
        //����Ŀ���ַ    Setting the destination address
        DL_DMA_setDestAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t)(&UART_1_INST->TXDATA));
    
        //����Ҫ���˵��ֽ��� Set the number of bytes to be carried
        DL_DMA_setTransferSize(DMA, DMA_CH0_CHAN_ID, size);
    
        //ʹ��DMAͨ��   Enable DMA Channel
        DL_DMA_enableChannel(DMA, DMA_CH0_CHAN_ID);
    
        gConsoleTxTransmitted    = false;
        gConsoleTxDMATransmitted = false;
    }
 
}

/*  �����жϽ��մ��� */
/* Serial port interrupt reception processing */
void UART_1_INST_IRQHandler(void)
{
	uint8_t Rx2_Temp = 0;
	
	switch( DL_UART_getPendingInterrupt(UART_1_INST) )
	{
//		case DL_UART_IIDX_RX://����ǽ����ж�	If it is a receive interrupt
//			
//			// ���շ��͹��������ݱ���	Receive and save the data sent
//			Rx2_Temp = DL_UART_Main_receiveData(UART_1_INST);
//			//����	deal with
//			Deal_Control_Rxtemp(Rx2_Temp);
//			break;
		
        case DL_UART_MAIN_IIDX_EOT_DONE:    //���ڷ������    Serial port send complete
            gConsoleTxTransmitted = true;
            break;
        
        case DL_UART_MAIN_IIDX_DMA_DONE_TX: //DMA�������   DMA handling complete
            gConsoleTxDMATransmitted = true;
            break;
        
        
		default://�����Ĵ����ж�	Other serial port interrupts
			break;
	}	
    

}
