#include "visual.h"
#include "mode.h"
#include "string.h"

/* --- �ⲿ���� --- */
extern uint8_t Visual_Data[25]; // ���� main.c �ġ����ж�������������֡
extern MODE mode;               // ȫ��״̬�ṹ��

/* --- ģ���ڲ���̬���� --- */

// ����״̬�������洢ѧϰ���ġ���ʼ���֡���-1 ��ʾ��δѧϰ��
static int s_initial_digit = -1;

// ���ڼ������½��ء��ĸ�������
static bool s_previous_sampling_command = false;

// ѧϰ�׶α���
static int s_initial_sample_count = 0;
static uint8_t s_initial_samples[INITIAL_SAMPLING_COUNT][MAX_DIGITS_PER_FRAME];
static uint8_t s_initial_sample_lengths[INITIAL_SAMPLING_COUNT];

// �����׶α���
#define NAV_MAX_SAMPLES 200 // Ϊ�����׶εĵ��������趨һ��������������
static int s_nav_sample_count = 0;
static uint8_t s_nav_samples[NAV_MAX_SAMPLES][MAX_DIGITS_PER_FRAME];
static uint8_t s_nav_sample_lengths[NAV_MAX_SAMPLES];


/* --- �ڲ��������� --- */

/**
 * @brief ͨ�õ�����Ƶ�ʷ����������ҳ������г��ִ��������������С�
 * @param samples_buffer ������������
 * @param lengths_buffer ������������
 * @param count ��������
 * @param out_sequence ���ڴ�����Ƶ��������
 * @param out_len ���ڴ�����Ƶ�����еĳ���
 * @return ��Ƶ�����г��ֵĴ���
 */
static int find_most_frequent_sequence(
    uint8_t samples_buffer[][MAX_DIGITS_PER_FRAME],
    const uint8_t lengths_buffer[],
    int count,
    uint8_t* out_sequence,
    uint8_t* out_len)
{
    if (count == 0) return 0;

    int max_freq = 0;
    for (int i = 0; i < count; i++) {
        if (lengths_buffer[i] == 0) continue;
        int current_freq = 0;
        for (int j = 0; j < count; j++) {
            if (lengths_buffer[i] == lengths_buffer[j] &&
                memcmp(samples_buffer[i], samples_buffer[j], lengths_buffer[i]) == 0) {
                current_freq++;
            }
        }
        if (current_freq > max_freq) {
            max_freq = current_freq;
            *out_len = lengths_buffer[i];
            memcpy(out_sequence, samples_buffer[i], *out_len);
        }
    }
    return max_freq;
}

/**
 * @brief ��������ʼѧϰ�����ȶ��ԡ�
 */
static bool analyze_initial_stability(uint8_t* out_digit)
{
    uint8_t most_frequent_sequence[MAX_DIGITS_PER_FRAME];
    uint8_t most_frequent_len = 0;
    int max_freq = find_most_frequent_sequence(s_initial_samples, s_initial_sample_lengths, s_initial_sample_count, most_frequent_sequence, &most_frequent_len);

    // ����Ƿ�������ֵ�����ұ����ǵ�������(1-8)
    if (max_freq >= INITIAL_STABILITY_THRESHOLD && most_frequent_len == 1 && most_frequent_sequence[0] >= 1 && most_frequent_sequence[0] <= 8) {
        *out_digit = most_frequent_sequence[0];
        return true;
    }
    return false;
}

/**
 * @brief ���������������ȶ��ԡ�
 */
static bool analyze_nav_stability(uint8_t* out_sequence, uint8_t* out_len)
{
    if (s_nav_sample_count == 0) return false;
    
    int max_freq = find_most_frequent_sequence(s_nav_samples, s_nav_sample_lengths, s_nav_sample_count, out_sequence, out_len);

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
    memset(s_initial_samples, 0, sizeof(s_initial_samples));
    memset(s_initial_sample_lengths, 0, sizeof(s_initial_sample_lengths));
    
    s_nav_sample_count = 0;
    memset(s_nav_samples, 0, sizeof(s_nav_samples));
    memset(s_nav_sample_lengths, 0, sizeof(s_nav_sample_lengths));
}
void visual_full_reset(void)
{
    s_initial_digit = -1;
    s_previous_sampling_command = false;
    visual_reset_sampling_status();
}


void visual_process_command(bool* sampling_command)
{
    // --- ���½��ء���⣺�������� true -> false ʱ��ִ�С������׶Ρ������շ��� ---
    if (*sampling_command == false && s_previous_sampling_command == true)
    {
        if (s_initial_digit != -1) { // ֻ������ѧϰ����ʼ���ֺ�����½��ز�������
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

    // --- �����ռ��׶Σ�������Ϊ true ʱִ�� ---
    if (*sampling_command == true)
    {
        uint8_t current_sequence[MAX_DIGITS_PER_FRAME];
        uint8_t current_length = 0;
        if (Visual_Data[0] == FRAME_HEADER && Visual_Data[Visual_Data[1] + 2] == FRAME_TRAILER) {
            current_length = Visual_Data[1];
            if (current_length > 0 && current_length <= MAX_DIGITS_PER_FRAME) {
                memcpy(current_sequence, &Visual_Data[2], current_length);
            } else {
                current_length = 0;
            }
        }
        if (current_length == 0) { // ���֡��Ч��ֱ�Ӹ���״̬������
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
            
            if (s_initial_sample_count >= INITIAL_SAMPLING_COUNT) {
                uint8_t learned_digit;
                if (analyze_initial_stability(&learned_digit)) {
                    s_initial_digit = learned_digit;
                    if (learned_digit == 1) mode.dir = LEFT;
                    else if (learned_digit == 2) mode.dir = RIGHT;
                    else mode.dir = FORWARD;
                } else {
                    mode.dir = UNSTABLE;
                }
                *sampling_command = false; // ������ɣ��Զ��������־λ��Ϊ false
            }
        } else {
            /***** �����׶� (ֻ�ռ����ݣ��ȴ��ֶ�����) *****/
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