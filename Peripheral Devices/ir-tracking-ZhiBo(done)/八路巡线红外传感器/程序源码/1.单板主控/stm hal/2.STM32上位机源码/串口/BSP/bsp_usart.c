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

// HAL��ĵײ��ʼ������������HAL_UART_Init()�б�����
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(huart->Instance == USART1)
  {
    /* Peripheral clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE(); // ����GPIOAʱ��

    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL; // ��������
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 1);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  }
  else if(huart->Instance == USART2)
  {
    /* Peripheral clock enable */
    __HAL_RCC_USART2_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE(); // ����GPIOAʱ��

    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL; // ��������
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART2 interrupt Init */
    HAL_NVIC_SetPriority(USART2_IRQn, 1, 1);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
  }
}

// ����һ���ֽ�
void USART1_Send_U8(uint8_t ch)
{
  HAL_UART_Transmit(&huart1, &ch, 1, 0xFFFF); // 0xFFFF�ǳ�ʱʱ��
}

// �����ֽ�����
void USART1_Send_ArrayU8(uint8_t *BufferPtr, uint16_t Length)
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
    USART1_Send_U8(Rx1_Temp); // ����
  }
  else if(huart->Instance == USART2)
  {
    uint8_t Rx2_Temp = 0; // ��������HAL_UART_Receive_IT�н��յ�һ���ֽ�
    // ��UART��������
    // HAL_UART_Receive_IT(&huart2, &Rx2_Temp, 1); // ������������
    Deal_IR_Usart(Rx2_Temp); // ��������
    // USART2_Send_U8(Rx2_Temp); // ֻ�շ�����
  }
}


// USART2��ʼ��
void USART2_init(u32 baudrate)
{
  huart2.Instance = USART2;
  huart2.Init.BaudRate = baudrate;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;

  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    // ��ʼ��������
    while(1);
  }
  // ����USART2�����жϣ�ͨ���ڳ�ʼ������������
  // �������ÿ��ֻ����һ���ֽڣ����ڻص���������������
  // ��main�����������ط����� HAL_UART_Receive_IT(&huart2, &Rx2_Temp_Buffer, 1);
  // ע�⣺Rx2_Temp_Buffer��Ҫ��ȫ�ֱ������߾�̬����������ͨ��������ʽ����
}


// ����һ���ַ�
void USART2_Send_U8(uint8_t ch)
{
  HAL_UART_Transmit(&huart2, &ch, 1, 0xFFFF);
}

// ����һ���ַ���
void USART2_Send_ArrayU8(uint8_t *BufferPtr, uint16_t Length)
{
  HAL_UART_Transmit(&huart2, BufferPtr, Length, 0xFFFF);
}

// USART2�жϷ�����
// ͬ��������Ҫ��stm32f1xx_it.c���ҵ�USART2_IRQHandler����ȷ����������HAL_UART_IRQHandler(&huart2);
// ����Ľ������ݴ�����HAL_UART_RxCpltCallback����ɡ�