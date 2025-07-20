#include "visual.h"
#include "mode.h"
#include "string.h" // 需要包含 string.h 来使用 memset 和 memcpy

/* --- 外部变量 --- */
extern uint8_t Visual_Data[25]; // 来自 main.c 的、由中断填充的最新数据帧
extern MODE mode;               // 来自 mode.c 的全局状态结构体

/* --- 模块内部静态变量，用于管理状态 --- */
// 状态标志
static int s_sample_count = 0;      // 当前已采样次数
static bool s_is_processing = false; // 内部标志，为 true 表示正在处理一个10次的采样任务

// 采样数据存储
static uint8_t s_sample_sequences[TARGET_SAMPLING_COUNT][MAX_DIGITS_PER_FRAME];
static uint8_t s_sample_lengths[TARGET_SAMPLING_COUNT];

// 最终分析结果
static uint8_t s_target_sequence[MAX_DIGITS_PER_FRAME];
static uint8_t s_target_digit_count = 0;


/* --- 内部辅助函数 --- */

/**
 * @brief 分析10个样本的稳定性，并得出最终的目标序列。
 * @return true 如果数据稳定, false 如果不稳定。
 */
static bool nav_analyze_stability(void)
{
    // 1. 确定基准长度（以第一次有效采样的长度为准）
    uint8_t base_length = 0;
    for(int i = 0; i < TARGET_SAMPLING_COUNT; ++i) {
        if(s_sample_lengths[i] > 0) {
            base_length = s_sample_lengths[i];
            break;
        }
    }

    if (base_length == 0) return false; // 没有任何有效样本
    s_target_digit_count = base_length;

    // 2. 对每一个位置的数字进行稳定性判断
    for (int pos = 0; pos < base_length; pos++)
    {
        int freq[10] = {0}; // 统计0-9的频率
        for (int i = 0; i < TARGET_SAMPLING_COUNT; i++)
        {
            if (s_sample_lengths[i] == base_length)
            {
                freq[s_sample_sequences[i][pos]]++;
            }
        }

        // 3. 寻找该位置出现最多次的数字
        int max_count = 0;
        int most_frequent_digit = -1;
        for (int digit = 0; digit < 10; digit++)
        {
            if (freq[digit] > max_count)
            {
                max_count = freq[digit];
                most_frequent_digit = digit;
            }
        }

        // 4. 检查是否满足稳定性阈值
        if (max_count < STABILITY_THRESHOLD)
        {
            return false; // 只要有一个位置不稳定，整体就不稳定
        }
        s_target_sequence[pos] = most_frequent_digit;
    }
    
    return true; // 所有位置都稳定
}

/**
 * @brief 根据分析出的稳定目标和当前视野，决定导航方向。
 * @return 导航方向 DIR
 */
static DIR visual_get_navigation_decision(void)
{
    // 解析最新的视野数据
    uint8_t visible_rooms[MAX_DIGITS_PER_FRAME];
    uint8_t visible_room_count = 0;
    if (Visual_Data[0] == FRAME_HEADER) {
        uint8_t data_length = Visual_Data[1];
        if (Visual_Data[data_length + 2] == FRAME_TRAILER && data_length <= MAX_DIGITS_PER_FRAME) {
            visible_room_count = data_length;
            memcpy(visible_rooms, &Visual_Data[2], data_length);
        }
    }
    
    // 如果当前视野无效或与目标长度不符，暂时直行
    if (visible_room_count != s_target_digit_count) return FORWARD;
    
    // 导航策略：寻找目标序列的第一个数字在当前视野中的位置
    for (int i = 0; i < visible_room_count; i++)
    {
        if (visible_rooms[i] == s_target_sequence[0])
        {
            // 简化策略：在视野左半边则左转，右半边则右转
            return (i < visible_room_count / 2) ? LEFT : RIGHT;
        }
    }

    return FORWARD; // 视野里没找到目标，直行
}


/* --- 对外公共函数实现 --- */

void visual_reset_state(void)
{
    s_sample_count = 0;
    s_is_processing = false; // 允许开始新的处理流程
    s_target_digit_count = 0;
    memset(s_sample_sequences, 0, sizeof(s_sample_sequences));
    memset(s_sample_lengths, 0, sizeof(s_sample_lengths));
    memset(s_target_sequence, 0, sizeof(s_target_sequence));
}

void visual_process_command(bool* sampling_command)
{
    // 如果主函数没有发出命令，或当前任务已完成，则直接返回
    if (*sampling_command == false || s_is_processing == false) {
        return;
    }
    
    // 如果已经收集满10个样本，则等待下一次 reset
    if (s_sample_count >= TARGET_SAMPLING_COUNT) {
        return;
    }

    // --- 后台数据收集中 ---
    // 解析当前帧数据
    uint8_t current_sequence[MAX_DIGITS_PER_FRAME];
    uint8_t current_length = 0;
    if (Visual_Data[0] == FRAME_HEADER) {
        uint8_t data_length = Visual_Data[1];
        if (Visual_Data[data_length + 2] == FRAME_TRAILER && data_length <= MAX_DIGITS_PER_FRAME) {
            current_length = data_length;
            memcpy(current_sequence, &Visual_Data[2], data_length);
        }
    }

    // 将有效数据存入样本库
    if (current_length > 0) {
        memcpy(s_sample_sequences[s_sample_count], current_sequence, current_length);
        s_sample_lengths[s_sample_count] = current_length;
        s_sample_count++;
    }

    // --- 决策与交付 ---
    // 当样本刚好收集满10次时，执行分析和交付
    if (s_sample_count >= TARGET_SAMPLING_COUNT)
    {
        if (nav_analyze_stability()) {
            // 如果稳定，则根据导航逻辑更新 mode.dir
            mode.dir = visual_get_navigation_decision();
            OLED_ShowString(0, 2, "Visual Sampling OK", 16);
        } else {
            // 如果不稳定，则更新 mode.dir 为不稳定状态
            mode.dir = UNSTABLE;
            OLED_ShowString(0, 2, "Visual Sampling Err", 16);
        }

        // 任务完成，重置命令标志位，并标记内部处理流程结束
        *sampling_command = false;
        s_is_processing = false; 
    }
}