#include "bsp_usart.h"
#include "string.h" // ������Ҫ���ͷ�ļ�

// ����UART���
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

// HAL��֧��printf�����ķ�����ͨ��������Ҫ__use_no_semihosting
// ����ͨ���ض���fputc��HAL��ķ��ͺ���
#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

PUTCHAR_PROTOTYPE
{
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF); // ͨ��UART1����
  return ch;
}


/**************************************************************************
Function: Serial port 1 initialization
Input   : bound��Baud rate
Output  : none
�������ܣ�����1��ʼ��
��ڲ�����bound��������
����  ֵ����
**************************************************************************/
void uart_init(u32 bound)
{
  huart1.Instance = USART1;
  huart1.Init.BaudRate = bound;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16; // STM32F1 HAL��ͨ������Ϊ16

  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    // ��ʼ�����������������ѭ���򱨴�
    while(1);
  }

  // ���ý����ж� (HAL���ͨ���ص���������)
  // HAL_UART_Receive_IT(&huart1, &Rx1_Temp, 1); // �������Ҫÿ���ֽڶ������жϣ���������������
}


// ����һ���ֽ�
void USART1_Send_uint8_t(uint8_t ch)
{
  HAL_UART_Transmit(&huart1, &ch, 1, 0xFFFF); // 0xFFFF�ǳ�ʱʱ��
}

// �����ֽ�����
void USART1_Send_Arrayuint8_t(uint8_t *BufferPtr, uint16_t Length)
{
  HAL_UART_Transmit(&huart1, BufferPtr, Length, 0xFFFF);
}

// ����1�жϷ����� (HAL����Զ����ûص�����)
// ����Ҫ��stm32f1xx_it.c���ҵ�USART1_IRQHandler����ȷ����������HAL_UART_IRQHandler(&huart1);
// ͨ����CubeMX���ɵĴ����У����Ѿ�Ϊ�������ˡ�
// ���յ����ݺ�Ĵ���ͨ��HAL��Ļص�����ʵ��
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if(huart->Instance == USART1)
  {
    uint8_t Rx1_Temp = 0; // ��������HAL_UART_Receive_IT�н��յ�һ���ֽ�
    // ��UART�������ݣ���ͨ���������жϽ���ʱ���Ѿ���ɣ�
    // HAL_UART_Receive_IT(&huart1, &Rx1_Temp, 1); // ������������
    // ������Ҫ������ʵ�ʵĽ��շ�ʽ������������жϽ��գ���Ӧ���Ѿ����жϻص��д����ˡ�
    // Ϊ�˼�����������������Ҫ�����յ����ֽ��������ͻ�ȥ
    USART1_Send_uint8_t(Rx1_Temp); // ����
  }
  else if(huart->Instance == USART2)
  {
    uint8_t Rx2_Temp = 0; // ��������HAL_UART_Receive_IT�н��յ�һ���ֽ�
    // ��UART��������
    // HAL_UART_Receive_IT(&huart2, &Rx2_Temp, 1); // ������������
    Deal_IR_Usart(Rx2_Temp); // ��������
    // USART2_Send_uint8_t(Rx2_Temp); // ֻ�շ�����
  }
}


// ����һ���ַ�
void USART2_Send_uint8_t(uint8_t ch)
{
  HAL_UART_Transmit(&huart2, &ch, 1, 0xFFFF);
}

// ����һ���ַ���
void USART2_Send_Arrayuint8_t(uint8_t *BufferPtr, uint16_t Length)
{
  HAL_UART_Transmit(&huart2, BufferPtr, Length, 0xFFFF);
}

// USART2�жϷ�����
// ͬ��������Ҫ��stm32f1xx_it.c���ҵ�USART2_IRQHandler����ȷ����������HAL_UART_IRQHandler(&huart2);
// ����Ľ������ݴ�����HAL_UART_RxCpltCallback����ɡ�