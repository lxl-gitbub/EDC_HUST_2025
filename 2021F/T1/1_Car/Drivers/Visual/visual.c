#include "visual.h"
#include "mode.h"
#include "string.h"

/* --- 外部变量 --- */
extern uint8_t Visual_Data[25];
extern MODE mode;

/* --- 模块内部静态变量 --- */
// 核心状态变量：存储学习到的“初始数字”。-1 表示还未学习。
static int s_initial_digit = -1;

// 采样过程变量
static int s_sample_count = 0;
static uint8_t s_sample_sequences[TARGET_SAMPLING_COUNT][MAX_DIGITS_PER_FRAME];
static uint8_t s_sample_lengths[TARGET_SAMPLING_COUNT];

// 最新一次稳定分析的结果
static uint8_t s_stable_sequence[MAX_DIGITS_PER_FRAME];
static uint8_t s_stable_digit_count = 0;


/* --- 内部辅助函数 --- */

/**
 * @brief 分析10个样本的稳定性，并将稳定结果存入静态变量。
 * @return true 如果数据稳定, false 如果不稳定。
 */
static bool nav_analyze_stability(void)
{
    uint8_t base_length = 0;
    for(int i = 0; i < TARGET_SAMPLING_COUNT; ++i) {
        if(s_sample_lengths[i] > 0) {
            base_length = s_sample_lengths[i];
            break;
        }
    }
    if (base_length == 0) return false;

    for (int pos = 0; pos < base_length; pos++) {
        int freq[10] = {0};
        for (int i = 0; i < TARGET_SAMPLING_COUNT; i++) {
            if (s_sample_lengths[i] == base_length) {
                freq[s_sample_sequences[i][pos]]++;
            }
        }
        int max_count = 0;
        int most_frequent_digit = -1;
        for (int digit = 0; digit < 10; digit++) {
            if (freq[digit] > max_count) {
                max_count = freq[digit];
                most_frequent_digit = digit;
            }
        }
        if (max_count < STABILITY_THRESHOLD) return false;
        s_stable_sequence[pos] = most_frequent_digit;
    }
    s_stable_digit_count = base_length;
    return true;
}


/* --- 对外公共函数实现 --- */

void visual_full_reset(void)
{
    s_initial_digit = -1;
    visual_reset_sampling_status();
}

void visual_reset_sampling_status(void)
{
    s_sample_count = 0;
    memset(s_sample_sequences, 0, sizeof(s_sample_sequences));
    memset(s_sample_lengths, 0, sizeof(s_sample_lengths));
    memset(s_stable_sequence, 0, sizeof(s_stable_sequence));
    s_stable_digit_count = 0;
}

void visual_process_command(bool* sampling_command)
{
    if (*sampling_command == false) return;
    if (s_sample_count >= TARGET_SAMPLING_COUNT) return;

    // --- 1. 收集数据 ---
    uint8_t current_sequence[MAX_DIGITS_PER_FRAME];
    uint8_t current_length = 0;
    if (Visual_Data[0] == FRAME_HEADER && Visual_Data[Visual_Data[1] + 2] == FRAME_TRAILER) {
        current_length = Visual_Data[1];
        if (current_length > 0 && current_length <= MAX_DIGITS_PER_FRAME) {
            memcpy(current_sequence, &Visual_Data[2], current_length);
            memcpy(s_sample_sequences[s_sample_count], current_sequence, current_length);
            s_sample_lengths[s_sample_count] = current_length;
            s_sample_count++;
        }
    }

    // --- 2. 当样本收集满10次后，执行分析和决策 ---
    if (s_sample_count >= TARGET_SAMPLING_COUNT)
    {
        if (nav_analyze_stability()) {
            // --- 采样成功且稳定 ---
            if (s_initial_digit == -1) {
                /***** 学习阶段 *****/
                // 只有当稳定结果是单个数字时，才学习
                if (s_stable_digit_count == 1) {
                    s_initial_digit = s_stable_sequence[0];
                    mode.dir = FORWARD; // 学习成功，给一个“OK”信号
                    OLED_ShowString(0, 2, "Initial Learned!", 16);
                } else {
                    // 第一次识别到的不是单个数字，视为不稳定
                    mode.dir = UNSTABLE;
                    OLED_ShowString(0, 2, "Initial Not Single", 16);
                }
            } else {
                /***** 导航比对阶段 *****/
                bool target_found = false;
                for (int i = 0; i < s_stable_digit_count; i++) {
                    if (s_stable_sequence[i] == s_initial_digit) {
                        mode.dir = (i < s_stable_digit_count / 2) ? LEFT : RIGHT;
                        target_found = true;
                        break;
                    }
                }
                if (!target_found) {
                    mode.dir = FORWARD; // 不包含初始数字，直行
                    OLED_ShowString(0, 2, "Keep Forward", 16);
                }
            }
        } else {
            // --- 采样失败或不稳定 ---
            mode.dir = UNSTABLE;
            OLED_ShowString(0, 2, "Visual UNSTABLE", 16);
        }

        // --- 任务完成，重置命令标志位 ---
        *sampling_command = false;
    }
}