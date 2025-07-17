#include "visual.h"
#include "mode.h"
// ��1


// ����״̬�ͻ�����
extern uint8_t USART_RX_BUF[USART_RX_BUF_LEN]; 
// �����������ݵĺ���
extern uint8_t arr[25];

extern MODE mode; // ȫ��ģʽ����
// ���淿��ŵ�����
uint8_t RoomArray[25] = {0} ;
// ������
int len ;
// ���÷������
DIR dir ;
// ȡ���õ�Ŀ�귿��
uint8_t goalRoomArray[100] = {0} ;
// Ŀ�귿��
int goalRoom = -1 ;

uint8_t sample_buffer[MAX_SAMPLES];
int sample_index = 0;
int sampling_done = 0; // ������ɱ�־


// ���ڵ���
// ����״̬�ͻ�����
extern uint8_t USART_EYE_RX_BUF[USART_EYE_RX_LEN]; 
// �����������ݵĺ���

// ����

void setRoomArray(void)
{
    if (arr[0] != 0x12) return; // ֡ͷУ��
	
    len = arr[1];

    if (arr[len + 2] != 0x5B) return; // ֡βУ�飨+2����Ϊ arr[0] ��֡ͷ��arr[1] �ǳ��ȣ�

    RoomArray[0] = len; // �������ݸ���
	
    for (int i = 0; i < len; i++) 
	{
        RoomArray[i + 1] = (int)arr[i + 2]; // �� arr[2] ��ʼ�����ݶ�
    }
}

/*
num = room 
if 1
 dir = left
elif 2
 dir = right
else
  dir = forward
	num = room
  if ar[] has num
		if x_num < len(ar) / 2
*/
void control()
{
	// ��������
	if (RoomArray[0] == 1 && RoomArray[1] == 1)
	{
		mode.dir = LEFT ;
	}
	else if (RoomArray[0] == 1 &&RoomArray[1] == 2)
	{
		mode.dir = RIGHT ;
	}
	else 
	{
		mode.dir = FORWARD ;
		if (RoomArray[0]>0)
		{
      // �������
      int isFound = 0;
			for (int j = 0 ; j < RoomArray[0] ; j ++)
			{
				if (goalRoom == RoomArray[j])
				{
          isFound = 1; // �ҵ�Ŀ�귿��
					if (j < RoomArray[0] / 2)
					{
						mode.dir = LEFT ;
					}
					else 
					{
						mode.dir = RIGHT ;
					}
				}
			}
      if (!isFound) // ���û���ҵ�Ŀ�귿��
      {
        mode.dir = FORWARD ; // Ĭ��ǰ��
      }
		}
	}
}

// ���ò���
void ResetSampling()
{
    sample_index = 0;
    sampling_done = 0;
}

// ��������Ϊ10
void SampleGoalNumFromArr(uint8_t arr[])
{
    if (sampling_done) return; // �����������ٲ���

    int len = arr[1];
	
    if (len == 1)
    {
        uint8_t val = arr[2]; // ����Ƿ񳬱�
        if (val <= 9)
        {
			
            if (sample_index < MAX_SAMPLES )
            {
                sample_buffer[sample_index++] = val;
				
            }

            if (sample_index >= MAX_SAMPLES )
            {
                int freq[10] = {0};
                for (int i = 0; i < MAX_SAMPLES ; i++)
                {
                    freq[sample_buffer[i]]++;
                }

                int maxCount = 0;
                int maxNum = 0;
                for (int i = 0; i < 10; i++)
                {
                    if (freq[i] > maxCount)
                    {
                        maxCount = freq[i];
                        maxNum = i;
                    }
                }
                goalRoom = maxNum;    // ����Ŀ������
                sampling_done = 1;    // ��ǲ������
            }
        }
    }
}
