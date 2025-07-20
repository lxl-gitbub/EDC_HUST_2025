#ifndef VISUAL_H
#define VISUAL_H

#include "AllHeader.h" // ��������� DIR, MODE �ȶ���
#include <stdbool.h>  // ���� bool ����

/* --- �궨�� --- */
#define FRAME_HEADER 0x12
#define FRAME_TRAILER 0x5B
#define Visual_Rx_Buff_Len 25 // ���ջ���������

#define TARGET_SAMPLING_COUNT 10  // ��Ҫ�����Ĵ���
#define MAX_DIGITS_PER_FRAME 4    // ÿ֡���ʶ������ָ���
#define STABILITY_THRESHOLD 6     // �ȶ�����ֵ��10�β����У�һ����������Ҫ�ȶ�������ô���


/* --- ���⹫������ --- */

/**
 * @brief �Ӿ����ݴ��������ں�����
 * @note  �˺���Ӧ��UART�����жϻص��������á������������־λ�����ں�ִ̨�в����ͷ�������
 * @param sampling_command ָ���������еġ���ʼ��������־λ��ָ�롣ģ����������Ὣ����Ϊ false��
 */
void visual_process_command(bool* sampling_command);

/**
 * @brief �����Ӿ�ģ����ڲ�����״̬��
 * @note  ����������ÿ�η����µ��Ӿ�����ǰ����Ӧ���ô˺�������վ����ݺ�״̬��
 */
void visual_reset_state(void);


#endif // VISUAL_H