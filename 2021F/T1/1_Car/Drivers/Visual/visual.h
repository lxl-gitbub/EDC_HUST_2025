#ifndef VISUAL_H
#define VISUAL_H

#include "AllHeader.h"
#include <stdbool.h>

/* --- �궨�� --- */
#define FRAME_HEADER 0x12
#define FRAME_TRAILER 0x5B
#define Visual_Rx_Buff_Len 25

#define TARGET_SAMPLING_COUNT 10
#define MAX_DIGITS_PER_FRAME 4
#define STABILITY_THRESHOLD 6 // �ȶ�����ֵ��10���������ȶ�����6��

/* --- ���⹫������ --- */

/**
 * @brief �Ӿ����ݴ��������ں�����
 * @note  ��UART�жϵ��á���������Ƿ���ѧϰ����ʼ���֡����Զ�ִ��ѧϰ�򵼺��ȶ�����
 * @param sampling_command ָ���������еġ���ʼ��������־λ��ָ�롣
 */
void visual_process_command(bool* sampling_command);

/**
 * @brief �����Ӿ�ģ��Ĳ���״̬��
 * @note  ����������ÿ�η����µ��Ӿ�����ǰ���á�
 * ��Ҫ���˺�������������ѧϰ�ġ���ʼ���֡���ֻ�����ò�����������
 */
void visual_reset_sampling_status(void);

/**
 * @brief ��ȫ�����Ӿ�ģ������״̬��������ѧϰ�ġ���ʼ���֡���
 * @note  ������������ʼʱ������Ҫ��������ѧϰʱ���á�
 */
void visual_full_reset(void);


#endif // VISUAL_H