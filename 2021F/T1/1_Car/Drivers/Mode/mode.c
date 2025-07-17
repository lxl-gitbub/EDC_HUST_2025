#include "mode.h"

bool drugSet(MODE* mode)
{
    if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) == GPIO_PIN_RESET && mode->loc == ZERO)
    {
        HAL_Delay(10); // ��������ʱ
        if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) == GPIO_PIN_RESET) // �ٴ�ȷ��״̬
        {
            mode->drug = PROPEL_MODE; // ��ҩģʽ
            mode->loc = ZERO; // λ�ù���
            return true;
        }
    }
    else if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) == GPIO_PIN_SET && mode->loc != ZERO)
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

DIR DirGet(MODE* mode)
{
    // ���ݵ�ǰģʽ��λ�÷�����һ������, ͬʱ���mode��״̬���и���
    if(mode->drug == PROPEL_MODE)
    {
        if(mode->dir == LEFT)
        {
            switch (mode->loc)
            {
                case ZERO:
                    mode->loc = L1;
                    return LEFT; // ����ת
                case ONE:
                    mode->loc = L2;
                    return LEFT; // ����ת
                case TWO:
                    mode->loc = L3;
                    return LEFT; // ����ת
                case L3:
                    mode->loc = L3_L;
                    return LEFT; // ����ת
                case R3:
                    mode->loc = R3_L;
                    return LEFT; // ����ת
            }
        }  
        else if(mode->dir == RIGHT)
        {
            switch (mode->loc)
            {
                case ZERO:
                    mode->loc = R1;
                    return RIGHT; // ����ת
                case ONE:
                    mode->loc = R2;
                    return RIGHT; // ����ת
                case TWO:
                    mode->loc = R3;
                    return RIGHT; // ����ת
                case L3:
                    mode->loc = L3_R;
                    return RIGHT; // ����ת
                case R3:
                    mode->loc = R3_R;
                    return RIGHT; // ����ת
            }
        }
        else if(mode->dir == FORWARD)
        {
            switch (mode->loc)
            {
                case ZERO:
                    mode->loc = ONE;
                    return FORWARD; // ��ǰ��
                case ONE:
                    mode->loc = TWO;
                    return FORWARD; // ��ǰ��
            }
        }
    }
    else if(mode->drug == RETURN_MODE)
    {
        switch( mode->loc )
        {
            case L1:
                mode->loc = ZERO;
                return LEFT; // ����ת
            case L2:
                mode->loc = ONE;
                return LEFT; // ����ת
            case R1:
                mode->loc = ZERO;
                return RIGHT; // ����ת
            case R2:
                mode->loc = ONE;
                return RIGHT; // ����ת
            case L3_L:
                mode->loc = L3;
                return LEFT; // ����ת
            case L3_R:
                mode->loc = L3;
                return RIGHT; // ����ת
            case R3_L:
                mode->loc = R3;
                return LEFT; // ����ת
            case R3_R:
                mode->loc = R3;
                return RIGHT; // ����ת
            case ONE:
                mode->loc = ZERO;
                return FORWARD; // ��ǰ��
            case TWO:
                mode->loc = ONE;
                return FORWARD; // ��ǰ��
            case L3:
                mode->loc = TWO;
                return RIGHT; // ����ת
            case R3:
                mode->loc = TWO;
                return LEFT; // ����ת
        }
    }
}