#ifndef VISUAL_H
#define VISUAL_H

#include "AllHeader.h" // 假设包含了 DIR, MODE 等定义
#include <stdbool.h>  // 引入 bool 类型

/* --- 宏定义 --- */
#define FRAME_HEADER 0x12
#define FRAME_TRAILER 0x5B
#define Visual_Rx_Buff_Len 25 // 接收缓冲区长度

#define TARGET_SAMPLING_COUNT 10  // 需要采样的次数
#define MAX_DIGITS_PER_FRAME 4    // 每帧最多识别的数字个数
#define STABILITY_THRESHOLD 6     // 稳定性阈值：10次采样中，一个数字至少要稳定出现这么多次


/* --- 对外公共函数 --- */

/**
 * @brief 视觉数据处理的主入口函数。
 * @note  此函数应由UART接收中断回调函数调用。它会检查命令标志位，并在后台执行采样和分析任务。
 * @param sampling_command 指向主函数中的“开始采样”标志位的指针。模块完成任务后会将其置为 false。
 */
void visual_process_command(bool* sampling_command);

/**
 * @brief 重置视觉模块的内部所有状态。
 * @note  在主函数中每次发起新的视觉任务前，都应调用此函数来清空旧数据和状态。
 */
void visual_reset_state(void);


#endif // VISUAL_H