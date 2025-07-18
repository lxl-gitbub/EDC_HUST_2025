#include "mode.h"

bool drugSet(MODE* mode)
{
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
    else if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) == GPIO_PIN_SET && mode->loc.n != 0)
    {
        HAL_Delay(10); // ��������ʱ
        if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) == GPIO_PIN_SET) // �ٴ�ȷ��״̬
        {
            mode->drug = RETURN_MODE; // �ع�ģʽ
            return true;
        }
    }
    return false; // û��ģʽ�仯
}

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

DIR ForToBack(DIR dir)
{
    // ��ǰ������ת��Ϊ���ط���
    switch(dir)
    {
        case LEFT:
            return RIGHT; // ��ת��Ϊ��ת
        case RIGHT:
            return LEFT; // ��ת��Ϊ��ת
        case FORWARD:
            return FORWARD; // ǰ������ǰ��
        default:
            return dir; // ����������ֲ���
    }
}

DIR DirGet(MODE* mode)
{
    // ��ȡ��һ������
   if(mode->drug == PROPEL_MODE)
    {
        mode->loc.trace[mode->loc.n++] = mode->dir; // ��¼��ǰ����
        return mode->dir; // ���ص�ǰ����
    }
    else if(mode->drug == RETURN_MODE)
    {
        if(isEndOfWay(mode->loc))
        {
            mode->loc.n --; // ���ټ�¼�ķ�����
            return mode->loc.trace[mode->loc.n]; // ������һ������
        }
        else
        {
            mode->loc.n --; // ���ټ�¼�ķ�����
            return ForToBack(mode->loc.trace[mode->loc.n]); // ����ת����ķ���
        }
    }
		return FORWARD;
}

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