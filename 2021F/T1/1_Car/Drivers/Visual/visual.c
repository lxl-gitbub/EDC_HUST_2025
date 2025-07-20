#include "visual.h"
#include "mode.h"
#include "string.h"

/* --- �ⲿ���� --- */
extern uint8_t Visual_Data[25];
extern MODE mode;

/* --- ģ���ڲ���̬���� --- */
// ����״̬�������洢ѧϰ���ġ���ʼ���֡���-1 ��ʾ��δѧϰ��
static int s_initial_digit = -1;

// �������̱���
static int s_sample_count = 0;
static uint8_t s_sample_sequences[TARGET_SAMPLING_COUNT][MAX_DIGITS_PER_FRAME];
static uint8_t s_sample_lengths[TARGET_SAMPLING_COUNT];

// ����һ���ȶ������Ľ��
static uint8_t s_stable_sequence[MAX_DIGITS_PER_FRAME];
static uint8_t s_stable_digit_count = 0;


/* --- �ڲ��������� --- */

/**
 * @brief ����10���������ȶ��ԣ������ȶ�������뾲̬������
 * @return true ��������ȶ�, false ������ȶ���
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


/* --- ���⹫������ʵ�� --- */

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

    // --- 1. �ռ����� ---
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

    // --- 2. �������ռ���10�κ�ִ�з����;��� ---
    if (s_sample_count >= TARGET_SAMPLING_COUNT)
    {
        if (nav_analyze_stability()) {
            // --- �����ɹ����ȶ� ---
            if (s_initial_digit == -1) {
                /***** ѧϰ�׶� *****/
                // ֻ�е��ȶ�����ǵ�������ʱ����ѧϰ
                if (s_stable_digit_count == 1) {
                    s_initial_digit = s_stable_sequence[0];
                    mode.dir = FORWARD; // ѧϰ�ɹ�����һ����OK���ź�
                    OLED_ShowString(0, 2, "Initial Learned!", 16);
                } else {
                    // ��һ��ʶ�𵽵Ĳ��ǵ������֣���Ϊ���ȶ�
                    mode.dir = UNSTABLE;
                    OLED_ShowString(0, 2, "Initial Not Single", 16);
                }
            } else {
                /***** �����ȶԽ׶� *****/
                bool target_found = false;
                for (int i = 0; i < s_stable_digit_count; i++) {
                    if (s_stable_sequence[i] == s_initial_digit) {
                        mode.dir = (i < s_stable_digit_count / 2) ? LEFT : RIGHT;
                        target_found = true;
                        break;
                    }
                }
                if (!target_found) {
                    mode.dir = FORWARD; // ��������ʼ���֣�ֱ��
                    OLED_ShowString(0, 2, "Keep Forward", 16);
                }
            }
        } else {
            // --- ����ʧ�ܻ��ȶ� ---
            mode.dir = UNSTABLE;
            OLED_ShowString(0, 2, "Visual UNSTABLE", 16);
        }

        // --- ������ɣ����������־λ ---
        *sampling_command = false;
    }
}