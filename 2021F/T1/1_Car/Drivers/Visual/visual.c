#include "visual.h"
#include "mode.h"
#include "string.h" // ��Ҫ���� string.h ��ʹ�� memset �� memcpy

/* --- �ⲿ���� --- */
extern uint8_t Visual_Data[25]; // ���� main.c �ġ����ж�������������֡
extern MODE mode;               // ���� mode.c ��ȫ��״̬�ṹ��

/* --- ģ���ڲ���̬���������ڹ���״̬ --- */
// ״̬��־
static int s_sample_count = 0;      // ��ǰ�Ѳ�������
static bool s_is_processing = false; // �ڲ���־��Ϊ true ��ʾ���ڴ���һ��10�εĲ�������

// �������ݴ洢
static uint8_t s_sample_sequences[TARGET_SAMPLING_COUNT][MAX_DIGITS_PER_FRAME];
static uint8_t s_sample_lengths[TARGET_SAMPLING_COUNT];

// ���շ������
static uint8_t s_target_sequence[MAX_DIGITS_PER_FRAME];
static uint8_t s_target_digit_count = 0;


/* --- �ڲ��������� --- */

/**
 * @brief ����10���������ȶ��ԣ����ó����յ�Ŀ�����С�
 * @return true ��������ȶ�, false ������ȶ���
 */
static bool nav_analyze_stability(void)
{
    // 1. ȷ����׼���ȣ��Ե�һ����Ч�����ĳ���Ϊ׼��
    uint8_t base_length = 0;
    for(int i = 0; i < TARGET_SAMPLING_COUNT; ++i) {
        if(s_sample_lengths[i] > 0) {
            base_length = s_sample_lengths[i];
            break;
        }
    }

    if (base_length == 0) return false; // û���κ���Ч����
    s_target_digit_count = base_length;

    // 2. ��ÿһ��λ�õ����ֽ����ȶ����ж�
    for (int pos = 0; pos < base_length; pos++)
    {
        int freq[10] = {0}; // ͳ��0-9��Ƶ��
        for (int i = 0; i < TARGET_SAMPLING_COUNT; i++)
        {
            if (s_sample_lengths[i] == base_length)
            {
                freq[s_sample_sequences[i][pos]]++;
            }
        }

        // 3. Ѱ�Ҹ�λ�ó������ε�����
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

        // 4. ����Ƿ������ȶ�����ֵ
        if (max_count < STABILITY_THRESHOLD)
        {
            return false; // ֻҪ��һ��λ�ò��ȶ�������Ͳ��ȶ�
        }
        s_target_sequence[pos] = most_frequent_digit;
    }
    
    return true; // ����λ�ö��ȶ�
}

/**
 * @brief ���ݷ��������ȶ�Ŀ��͵�ǰ��Ұ��������������
 * @return �������� DIR
 */
static DIR visual_get_navigation_decision(void)
{
    // �������µ���Ұ����
    uint8_t visible_rooms[MAX_DIGITS_PER_FRAME];
    uint8_t visible_room_count = 0;
    if (Visual_Data[0] == FRAME_HEADER) {
        uint8_t data_length = Visual_Data[1];
        if (Visual_Data[data_length + 2] == FRAME_TRAILER && data_length <= MAX_DIGITS_PER_FRAME) {
            visible_room_count = data_length;
            memcpy(visible_rooms, &Visual_Data[2], data_length);
        }
    }
    
    // �����ǰ��Ұ��Ч����Ŀ�곤�Ȳ�������ʱֱ��
    if (visible_room_count != s_target_digit_count) return FORWARD;
    
    // �������ԣ�Ѱ��Ŀ�����еĵ�һ�������ڵ�ǰ��Ұ�е�λ��
    for (int i = 0; i < visible_room_count; i++)
    {
        if (visible_rooms[i] == s_target_sequence[0])
        {
            // �򻯲��ԣ�����Ұ��������ת���Ұ������ת
            return (i < visible_room_count / 2) ? LEFT : RIGHT;
        }
    }

    return FORWARD; // ��Ұ��û�ҵ�Ŀ�ֱ꣬��
}


/* --- ���⹫������ʵ�� --- */

void visual_reset_state(void)
{
    s_sample_count = 0;
    s_is_processing = false; // ����ʼ�µĴ�������
    s_target_digit_count = 0;
    memset(s_sample_sequences, 0, sizeof(s_sample_sequences));
    memset(s_sample_lengths, 0, sizeof(s_sample_lengths));
    memset(s_target_sequence, 0, sizeof(s_target_sequence));
}

void visual_process_command(bool* sampling_command)
{
    // ���������û�з��������ǰ��������ɣ���ֱ�ӷ���
    if (*sampling_command == false || s_is_processing == false) {
        return;
    }
    
    // ����Ѿ��ռ���10����������ȴ���һ�� reset
    if (s_sample_count >= TARGET_SAMPLING_COUNT) {
        return;
    }

    // --- ��̨�����ռ��� ---
    // ������ǰ֡����
    uint8_t current_sequence[MAX_DIGITS_PER_FRAME];
    uint8_t current_length = 0;
    if (Visual_Data[0] == FRAME_HEADER) {
        uint8_t data_length = Visual_Data[1];
        if (Visual_Data[data_length + 2] == FRAME_TRAILER && data_length <= MAX_DIGITS_PER_FRAME) {
            current_length = data_length;
            memcpy(current_sequence, &Visual_Data[2], data_length);
        }
    }

    // ����Ч���ݴ���������
    if (current_length > 0) {
        memcpy(s_sample_sequences[s_sample_count], current_sequence, current_length);
        s_sample_lengths[s_sample_count] = current_length;
        s_sample_count++;
    }

    // --- �����뽻�� ---
    // �������պ��ռ���10��ʱ��ִ�з����ͽ���
    if (s_sample_count >= TARGET_SAMPLING_COUNT)
    {
        if (nav_analyze_stability()) {
            // ����ȶ�������ݵ����߼����� mode.dir
            mode.dir = visual_get_navigation_decision();
            OLED_ShowString(0, 2, "Visual Sampling OK", 16);
        } else {
            // ������ȶ�������� mode.dir Ϊ���ȶ�״̬
            mode.dir = UNSTABLE;
            OLED_ShowString(0, 2, "Visual Sampling Err", 16);
        }

        // ������ɣ����������־λ��������ڲ��������̽���
        *sampling_command = false;
        s_is_processing = false; 
    }
}