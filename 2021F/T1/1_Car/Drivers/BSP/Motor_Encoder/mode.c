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
            mode->loc = ZERO; // λ�ù���
            return true;
        }
    }
    return false; // û��ģʽ�仯
}

DIR DirGet(MODE* mode)
{
    // ���ݵ�ǰģʽ��λ�÷�����һ������, ͬʱ���mode��״̬���и���
		mode->loc = L1;
    return LEFT; // ������Ը���ʵ���߼����ز�ͬ�ķ���
}