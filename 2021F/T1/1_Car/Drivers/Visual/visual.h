#ifndef VISUAL_H
#define VISUAL_H

#include "AllHeader.h"
#include <stdbool.h>

/* --- �궨�� --- */
#define FRAME_HEADER 0x12
#define FRAME_TRAILER 0x5B
#define Visual_Rx_Buff_Len 25

#define MAX_DIGITS_PER_FRAME 4

// --- ѧϰ�׶����� ---
#define INITIAL_SAMPLING_COUNT 10
#define INITIAL_STABILITY_THRESHOLD 6

// --- �����׶����� ---
// �ȶ����жϱ�׼���ڵ�������ʼ��ĳ�������ڶ�Ӧλ�ó��ֵĸ���Ҫ���ڵ������ֵ
#define NAV_STABILITY_PROBABILITY 0.6f // 60%


/* --- ���⹫������ --- */

/**
 * @brief �Ӿ����ݴ��������ں�����
 * @param sampling_command ָ���������еġ���ʼ��������־λ��ָ�롣
 */
void visual_process_command(bool* sampling_command);

/**
 * @brief ���õ����׶εĲ���״̬��
 * @note  ����������ÿ�η��𡰳�������������ǰ���á�
 */
void visual_reset_nav_status(void);

/**
 * @brief ��ȫ�����Ӿ�ģ������״̬��������ѧϰ�ġ���ʼ���֡���
 * @note  ������������ʼʱ���á�
 */
void visual_full_reset(void);


#endif // VISUAL_H