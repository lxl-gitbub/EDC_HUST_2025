#include "visual.h"
#include "mode.h"
#include "string.h"

/* --- �ⲿ���� --- */
extern uint8_t Visual_Data[25];
extern MODE mode;

/* --- ģ���ڲ���̬���� --- */
// ����״̬
static int s_initial_digit = -1;
static bool s_previous_sampling_command = false; // ���ڼ������ġ��½��ء�

// ѧϰ�׶α���
static int s_initial_sample_count = 0;
static uint8_t s_initial_samples[INITIAL_SAMPLING_COUNT][MAX_DIGITS_PER_FRAME];
static uint8_t s_initial_sample_lengths[INITIAL_SAMPLING_COUNT];

// �����׶α���
#define NAV_MAX_SAMPLES 200 // �����׶�����ռ�50������
static int s_nav_sample_count = 0;
static uint8_t s_nav_samples[NAV_MAX_SAMPLES][MAX_DIGITS_PER_FRAME];
static uint8_t s_nav_sample_lengths[NAV_MAX_SAMPLES];


/* --- �ڲ��������� --- */

/**
 * @brief [���߼�] ��������ʼѧϰ�����ȶ��ԣ������������е�Ƶ�ʡ�
 * @param out_digit ����ȶ����ǵ������֣���ͨ����ָ�봫����
 * @return ����ȶ��򷵻� true��
 */
static bool analyze_initial_stability(uint8_t* out_digit)
{
    if (s_initial_sample_count == 0) return false;

    int max_freq = 0;
    uint8_t most_frequent_sequence[MAX_DIGITS_PER_FRAME];
    uint8_t most_frequent_len = 0;

    // �ҵ�����Ƶ����ߵ���������
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

    // ����Ƿ�������ֵ�����ұ����ǵ�������
    if (max_freq >= INITIAL_STABILITY_THRESHOLD && most_frequent_len == 1) {
        *out_digit = most_frequent_sequence[0];
        return true;
    }
    return false;
}

/**
 * @brief [���߼�] ���������������ȶ��ԣ������������е�Ƶ�ʡ�
 * @param out_sequence ����ȶ���ͨ����ָ�봫���ȶ����С�
 * @param out_len �����ȶ����еĳ��ȡ�
 * @return ����ȶ��򷵻� true��
 */
static bool analyze_nav_stability(uint8_t* out_sequence, uint8_t* out_len)
{
    if (s_nav_sample_count == 0) return false;
    
    int max_freq = 0;
    // ... (Ѱ����Ƶ�����е��߼������� analyze_initial_stability �е�һ��) ...
    // ... Ϊ���ʡ���ظ����룬ʵ��Ӧ���п��Խ������Ϊһ��ͨ�ú��� ...
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
    
    // ����Ƿ����������ֵ
    if (((float)max_freq / s_nav_sample_count) >= NAV_STABILITY_PROBABILITY) {
        return true;
    }
    return false;
}


/* --- ���⹫������ʵ�� --- */

void visual_reset_sampling_status(void)
{
    s_initial_sample_count = 0;
    s_nav_sample_count = 0;
    // ... ���� memset ���� ...
}

void visual_full_reset(void)
{
    s_initial_digit = -1;
    s_previous_sampling_command = false;
    visual_reset_sampling_status();
}

void visual_process_command(bool* sampling_command)
{
    // --- ���½��ء���⣺�������� true -> false ʱ��ִ�����շ��� ---
    if (*sampling_command == false && s_previous_sampling_command == true)
    {
        if (s_initial_digit == -1) {
            /***** ѧϰ�׶ε����շ��� *****/
            uint8_t learned_digit;
            if (analyze_initial_stability(&learned_digit)) {
                s_initial_digit = learned_digit;
                mode.dir = FORWARD; // ѧϰ�ɹ�
                OLED_ShowString(10, 2, "Visual Sussess",16);
            } else {
                mode.dir = UNSTABLE; // ѧϰʧ��
                OLED_ShowString(10, 2, "Visual Failed",16);
            }
        } else {
            /***** �����׶ε����շ��� *****/
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
        // ��ʼ���������ռ��׶�
        visual_reset_sampling_status();
    }

   // --- �����ռ��׶Σ�������Ϊ true ʱִ�� ---
    if (*sampling_command == true)
    {
        uint8_t current_sequence[MAX_DIGITS_PER_FRAME];
        uint8_t current_length = 0;
        // ���� Visual_Data ����
        if (Visual_Data[0] == FRAME_HEADER && Visual_Data[Visual_Data[1] + 2] == FRAME_TRAILER) {
            current_length = Visual_Data[1];
            if (current_length > 0 && current_length <= MAX_DIGITS_PER_FRAME) {
                memcpy(current_sequence, &Visual_Data[2], current_length);
            } else {
                current_length = 0;
            }
        }
        if (current_length == 0) { // ���֡��Ч��ֱ�Ӹ�����һ�ε�״̬������
             s_previous_sampling_command = *sampling_command;
             return;
        }

        // --- �����Ƿ���ѧϰ���ַ����� ---
        if (s_initial_digit == -1) {
            /***** ѧϰ�׶� (�Զ�����) *****/
            if (s_initial_sample_count < INITIAL_SAMPLING_COUNT) {
                memcpy(s_initial_samples[s_initial_sample_count], current_sequence, current_length);
                s_initial_sample_lengths[s_initial_sample_count] = current_length;
                s_initial_sample_count++;
            }
            // �����㣺�������ռ���10�κ������������Զ�����
            if (s_initial_sample_count >= INITIAL_SAMPLING_COUNT) {
                uint8_t learned_digit;
                if (analyze_initial_stability(&learned_digit)) {
                    s_initial_digit = learned_digit;
                    mode.dir = FORWARD; // ѧϰ�ɹ�����OK�ź�
                    OLED_ShowString(10, 2, "Initial Success",16);
                } else {
                    mode.dir = UNSTABLE; // ѧϰʧ��
                    OLED_ShowString(10, 2, "Initial Failed",16);
                }
                *sampling_command = false; // ������ɣ��Զ��������־λ��Ϊ false
            }
        } else {
            /***** �����׶� (ֻ�ռ�����) *****/
            if (s_nav_sample_count < NAV_MAX_SAMPLES) {
                memcpy(s_nav_samples[s_nav_sample_count], current_sequence, current_length);
                s_nav_sample_lengths[s_nav_sample_count] = current_length;
                s_nav_sample_count++;
            }
        }
    }

    // �ں�������󣬸�����һ�ε�����״̬��Ϊ��һ�μ����׼��
    s_previous_sampling_command = *sampling_command;
}