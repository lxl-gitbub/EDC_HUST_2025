#include "visual.h"
#include "mode.h"
#include "string.h"

/* --- 外部变量 --- */
extern uint8_t Visual_Data[25];
extern MODE mode;

/* --- 模块内部静态变量 --- */
// 核心状态
static int s_initial_digit = -1;
static bool s_previous_sampling_command = false; // 用于检测命令的“下降沿”

// 学习阶段变量
static int s_initial_sample_count = 0;
static uint8_t s_initial_samples[INITIAL_SAMPLING_COUNT][MAX_DIGITS_PER_FRAME];
static uint8_t s_initial_sample_lengths[INITIAL_SAMPLING_COUNT];

// 导航阶段变量
#define NAV_MAX_SAMPLES 200 // 导航阶段最多收集50个样本
static int s_nav_sample_count = 0;
static uint8_t s_nav_samples[NAV_MAX_SAMPLES][MAX_DIGITS_PER_FRAME];
static uint8_t s_nav_sample_lengths[NAV_MAX_SAMPLES];


/* --- 内部辅助函数 --- */

/**
 * @brief [新逻辑] 分析“初始学习”的稳定性，基于完整序列的频率。
 * @param out_digit 如果稳定且是单个数字，则通过此指针传出。
 * @return 如果稳定则返回 true。
 */
static bool analyze_initial_stability(uint8_t* out_digit)
{
    if (s_initial_sample_count == 0) return false;

    int max_freq = 0;
    uint8_t most_frequent_sequence[MAX_DIGITS_PER_FRAME];
    uint8_t most_frequent_len = 0;

    // 找到出现频率最高的完整序列
    for (int i = 0; i < s_initial_sample_count; i++) {
        int current_freq = 0;
        for (int j = 0; j < s_initial_sample_count; j++) {
            if (s_initial_sample_lengths[i] == s_initial_sample_lengths[j] &&
                memcmp(s_initial_samples[i], s_initial_samples[j], s_initial_sample_lengths[i]) == 0) {
                current_freq++;
            }
        }
        if (current_freq > max_freq) {
            max_freq = current_freq;
            most_frequent_len = s_initial_sample_lengths[i];
            memcpy(most_frequent_sequence, s_initial_samples[i], most_frequent_len);
        }
    }

    // 检查是否满足阈值，并且必须是单个数字
    if (max_freq >= INITIAL_STABILITY_THRESHOLD && most_frequent_len == 1) {
        *out_digit = most_frequent_sequence[0];
        return true;
    }
    return false;
}

/**
 * @brief [新逻辑] 分析“导航”的稳定性，基于完整序列的频率。
 * @param out_sequence 如果稳定，通过此指针传出稳定序列。
 * @param out_len 传出稳定序列的长度。
 * @return 如果稳定则返回 true。
 */
static bool analyze_nav_stability(uint8_t* out_sequence, uint8_t* out_len)
{
    if (s_nav_sample_count == 0) return false;
    
    int max_freq = 0;
    // ... (寻找最频繁序列的逻辑和上面 analyze_initial_stability 中的一样) ...
    // ... 为简洁省略重复代码，实际应用中可以将其抽象为一个通用函数 ...
    for (int i = 0; i < s_nav_sample_count; i++) {
        int current_freq = 0;
        for (int j = 0; j < s_nav_sample_count; j++) {
            if (s_nav_sample_lengths[i] == s_nav_sample_lengths[j] &&
                memcmp(s_nav_samples[i], s_nav_samples[j], s_nav_sample_lengths[i]) == 0) {
                current_freq++;
            }
        }
        if (current_freq > max_freq) {
            max_freq = current_freq;
            *out_len = s_nav_sample_lengths[i];
            memcpy(out_sequence, s_nav_samples[i], *out_len);
        }
    }
    
    // 检查是否满足概率阈值
    if (((float)max_freq / s_nav_sample_count) >= NAV_STABILITY_PROBABILITY) {
        return true;
    }
    return false;
}


/* --- 对外公共函数实现 --- */

void visual_reset_sampling_status(void)
{
    s_initial_sample_count = 0;
    s_nav_sample_count = 0;
    // ... 其他 memset 操作 ...
}

void visual_full_reset(void)
{
    s_initial_digit = -1;
    s_previous_sampling_command = false;
    visual_reset_sampling_status();
}

void visual_process_command(bool* sampling_command)
{
    // --- “下降沿”检测：当命令由 true -> false 时，执行最终分析 ---
    if (*sampling_command == false && s_previous_sampling_command == true)
    {
        if (s_initial_digit == -1) {
            /***** 学习阶段的最终分析 *****/
            uint8_t learned_digit;
            if (analyze_initial_stability(&learned_digit)) {
                s_initial_digit = learned_digit;
                mode.dir = FORWARD; // 学习成功
                OLED_ShowString(10, 2, "Visual Sussess",16);
            } else {
                mode.dir = UNSTABLE; // 学习失败
                OLED_ShowString(10, 2, "Visual Failed",16);
            }
        } else {
            /***** 导航阶段的最终分析 *****/
            uint8_t stable_sequence[MAX_DIGITS_PER_FRAME];
            uint8_t stable_len;
            if (analyze_nav_stability(stable_sequence, &stable_len)) {
                bool target_found = false;
                for (int i = 0; i < stable_len; i++) {
                    if (stable_sequence[i] == s_initial_digit) {
                        mode.dir = (i < stable_len / 2) ? LEFT : RIGHT;
                        target_found = true;
                        break;
                    }
                }
                if (!target_found) mode.dir = FORWARD;
            } else {
                mode.dir = UNSTABLE;
            }
        }
    }
    else if (*sampling_command == true && s_previous_sampling_command == false)
    {
        // 开始进入数据收集阶段
        visual_reset_sampling_status();
    }

   // --- 数据收集阶段：当命令为 true 时执行 ---
    if (*sampling_command == true)
    {
        uint8_t current_sequence[MAX_DIGITS_PER_FRAME];
        uint8_t current_length = 0;
        // 解析 Visual_Data 数据
        if (Visual_Data[0] == FRAME_HEADER && Visual_Data[Visual_Data[1] + 2] == FRAME_TRAILER) {
            current_length = Visual_Data[1];
            if (current_length > 0 && current_length <= MAX_DIGITS_PER_FRAME) {
                memcpy(current_sequence, &Visual_Data[2], current_length);
            } else {
                current_length = 0;
            }
        }
        if (current_length == 0) { // 如果帧无效，直接更新上一次的状态并返回
             s_previous_sampling_command = *sampling_command;
             return;
        }

        // --- 根据是否已学习，分发任务 ---
        if (s_initial_digit == -1) {
            /***** 学习阶段 (自动结束) *****/
            if (s_initial_sample_count < INITIAL_SAMPLING_COUNT) {
                memcpy(s_initial_samples[s_initial_sample_count], current_sequence, current_length);
                s_initial_sample_lengths[s_initial_sample_count] = current_length;
                s_initial_sample_count++;
            }
            // 修正点：当样本收集满10次后，立即分析并自动结束
            if (s_initial_sample_count >= INITIAL_SAMPLING_COUNT) {
                uint8_t learned_digit;
                if (analyze_initial_stability(&learned_digit)) {
                    s_initial_digit = learned_digit;
                    mode.dir = FORWARD; // 学习成功，给OK信号
                    OLED_ShowString(10, 2, "Initial Success",16);
                } else {
                    mode.dir = UNSTABLE; // 学习失败
                    OLED_ShowString(10, 2, "Initial Failed",16);
                }
                *sampling_command = false; // 任务完成，自动将命令标志位置为 false
            }
        } else {
            /***** 导航阶段 (只收集数据) *****/
            if (s_nav_sample_count < NAV_MAX_SAMPLES) {
                memcpy(s_nav_samples[s_nav_sample_count], current_sequence, current_length);
                s_nav_sample_lengths[s_nav_sample_count] = current_length;
                s_nav_sample_count++;
            }
        }
    }

    // 在函数的最后，更新上一次的命令状态，为下一次检测做准备
    s_previous_sampling_command = *sampling_command;
}