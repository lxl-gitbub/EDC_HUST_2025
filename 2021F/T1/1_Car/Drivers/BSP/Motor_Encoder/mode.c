#include "mode.h"

bool drugSet(MODE* mode)
{
    if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) == GPIO_PIN_RESET && mode->loc == ZERO)
    {
        HAL_Delay(10); // 防抖动延时
        if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) == GPIO_PIN_RESET) // 再次确认状态
        {
            mode->drug = PROPEL_MODE; // 送药模式
            mode->loc = ZERO; // 位置归零
            return true;
        }
    }
    else if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) == GPIO_PIN_SET && mode->loc != ZERO)
    {
        HAL_Delay(10); // 防抖动延时
        if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) == GPIO_PIN_SET) // 再次确认状态
        {
            mode->drug = RETURN_MODE; // 回归模式
            mode->loc = ZERO; // 位置归零
            return true;
        }
    }
    return false; // 没有模式变化
}

DIR DirGet(MODE* mode)
{
    // 根据当前模式和位置返回下一个方向, 同时会对mode的状态进行更新
		mode->loc = L1;
    return LEFT; // 这里可以根据实际逻辑返回不同的方向
}