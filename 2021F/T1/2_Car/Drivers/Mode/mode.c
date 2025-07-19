#include "mode.h"

char command_From_Bluetooth = 0; // ���ڴ洢�������յ�������

LOC target_loc; // ���ڴ洢Ŀ��λ��
LOC waiting_loc;
const LOC FAR_WAITING_LOC = {2, {FORWARD, LEFT}}; // ��Զ����ҩʱ�ĵȴ�λ��
const LOC MIDDLE_WAITING_LOC = {2, {FORWARD, FORWARD}};

bool drugSet(MODE* mode)
{
    uint16_t change_state;
    if(command_From_Bluetooth == 0) // ���û�н��յ�����
    {
        return false; // �������κβ���
    }

    if(command_From_Bluetooth == 'M') // ������յ�'M'����,Ҳ������ҩ���в�
    {
        waiting_loc = FAR_WAITING_LOC;
        if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) == GPIO_PIN_RESET && mode->loc.n == 0)
        {
            HAL_Delay(10); // ��������ʱ
            if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) == GPIO_PIN_RESET) // �ٴ�ȷ��״̬
            {
                mode->drug = PROPEL_MODE; // ��ҩģʽ
                mode->loc.n = 0; // λ�ù���
                return true;
            }
        }
    }
    else if(command_From_Bluetooth == 'F') // ������յ�'F'����,Ҳ����ȡҩ��ǰ��
    {
        waiting_loc = MIDDLE_WAITING_LOC;
        mode->drug = PROPEL_MODE; // ��ҩģʽ
        mode->loc.n = 0; // λ�ù���
        return true;
    }
    return false; // û��ģʽ�仯
}

//���º��������ڻ�ȡ��һ�������ͬʱ����mode
bool isEndOfWay(LOC l)
{
    if(l.n == 1)
    {
        if(l.trace[0] == LEFT || l.trace[0] == RIGHT)
        {
            return true; // ֻ��һ��������������ң���ʾ�����յ�
        }
    }
    else if(l.n == 2)
    {
        if((l.trace[0] == FORWARD && l.trace[1] == LEFT) || 
           (l.trace[0] == FORWARD && l.trace[1] == RIGHT) )
        {
            return true; // ������������ǰ��������ң���ʾ�����յ�
        }
    }
    else if (l.n == 4)
    {
        if((l.trace[0] == FORWARD && l.trace[1] == FORWARD) &&
           (l.trace[2] != FORWARD && l.trace[3] != FORWARD) &&
           (l.trace[2] != BACKWARD && l.trace[3] != BACKWARD))
        {
            return true; // �ĸ�������ǰ�����Σ��������β���ǰ������ˣ���ʾ�����յ�
        }
    }
    return false; // ������������յ�
}
DIR DirGet(MODE* mode)
{
    // ��ȡ��һ������
    if(command_From_Bluetooth != 'C')//���Ǽ�����ҩ��ָ��,ǰ���ȴ��ص�
    {
        mode->loc.trace[mode->loc.n] = waiting_loc.trace[mode->loc.n];
        mode->loc.n++;
        return mode->loc.trace[mode->loc.n - 1];
    }
    if(command_From_Bluetooth == 'C')
    {
        if(mode->loc.n == 2 && 
           (mode->loc.trace[0] == FORWARD && mode->loc.trace[1] == FORWARD))
        {
            mode->loc.trace[mode->loc.n - 1] = target_loc.trace[mode->loc.n - 1];
            return target_loc.trace[mode->loc.n - 1]; // ����ǰ������
        }

        if(mode->loc.n == 2 && 
           (mode->loc.trace[0] == FORWARD && mode->loc.trace[1] == LEFT))
        {
            mode->loc.trace[mode->loc.n - 1] = FORWARD; // �����һ����������Ϊǰ��,��Ȼʵ������������ת�����ó���λ����ֱ����ֱ��
            return RIGHT; // ������ת����
        }
    }
    mode->loc.trace[mode->loc.n] = mode->dir; // ����ǰ�������λ�ü�¼
    mode->loc.n++; // ����λ�ü�¼������ 
	return mode->dir; // ���ص�ǰ����
}

//���ڸ���λ�û���λ��
float DirToTheta(DIR dir)
{
    // ������ת��Ϊ�Ƕ�
    switch(dir)
    {
        case LEFT:
            return 90.0f; // ��ת��Ӧ90��
        case RIGHT:
            return -90.0f; // ��ת��Ӧ-90��
        case FORWARD:
            return 0.0f; // ǰ����Ӧ0��
        case BACKWARD:
            return 180.0f; // ���˶�Ӧ180��
        default:
            return 0.0f; // Ĭ�Ϸ���0��
    }
}
float LocToTheta(LOC loc)
{
    float theta = 0.0f; // ��ʼ���Ƕ�
    // ��λ��ת��Ϊ�Ƕ�
    for(int i = 0; i < loc.n; i++)
    {
        theta += DirToTheta(loc.trace[i]); // �ۼ�ÿ�������Ӧ�ĽǶ�
    }
    return theta;
}

//���º������ڷ�������Ϣ�ж�location�Ĵ���
LOC BStringToLoc(char* str, int len)
{
    LOC loc = {0}; // ��ʼ��λ��
    loc.n = 0; // ��ʼλ����Ϊ0
    for(int i = 0; i < len && loc.n < 10; i++)
    {
        loc.trace[loc.n++] = str[i];
    } // ���ַ���ת��Ϊλ��
    return loc; // ����ת�����λ��
}