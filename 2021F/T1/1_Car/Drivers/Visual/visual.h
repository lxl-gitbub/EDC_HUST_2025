#ifndef VISUAL_H
#define VISUAL_H

#include "AllHeader.h"
#include <stdbool.h>

/* --- 宏定义 --- */
#define FRAME_HEADER 0x12
#define FRAME_TRAILER 0x5B
#define Visual_Rx_Buff_Len 25

#define TARGET_SAMPLING_COUNT 10
#define MAX_DIGITS_PER_FRAME 4
#define STABILITY_THRESHOLD 6 // 稳定性阈值：10次中至少稳定出现6次

/* --- 对外公共函数 --- */

/**
 * @brief 视觉数据处理的主入口函数。
 * @note  由UART中断调用。它会根据是否已学习“初始数字”，自动执行学习或导航比对任务。
 * @param sampling_command 指向主函数中的“开始采样”标志位的指针。
 */
void visual_process_command(bool* sampling_command);

/**
 * @brief 重置视觉模块的采样状态。
 * @note  在主函数中每次发起新的视觉任务前调用。
 * 重要：此函数不会重置已学习的“初始数字”，只会重置采样计数器。
 */
void visual_reset_sampling_status(void);

/**
 * @brief 完全重置视觉模块所有状态，包括已学习的“初始数字”。
 * @note  仅在整个程序开始时，或需要彻底重新学习时调用。
 */
void visual_full_reset(void);


#endif // VISUAL_H