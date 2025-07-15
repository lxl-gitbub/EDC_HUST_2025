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
            return true;
        }
    }
    return false; // 没有模式变化
}

DIR DirGet(MODE* mode)
{
    // 根据当前模式和位置返回下一个方向, 同时会对mode的状态进行更新
    if(mode->drug == PROPEL_MODE)
    {
        if(mode->dir == LEFT)
        {
            switch (mode->loc)
            {
                case ZERO:
                    mode->loc = L1;
                    return LEFT; // 向左转
                case ONE:
                    mode->loc = L2;
                    return LEFT; // 向左转
                case TWO:
                    mode->loc = L3;
                    return LEFT; // 向左转
                case L3:
                    mode->loc = L3_L;
                    return LEFT; // 向左转
                case R3:
                    mode->loc = R3_L;
                    return LEFT; // 向左转
            }
        }  
        else if(mode->dir == RIGHT)
        {
            switch (mode->loc)
            {
                case ZERO:
                    mode->loc = R1;
                    return RIGHT; // 向右转
                case ONE:
                    mode->loc = R2;
                    return RIGHT; // 向右转
                case TWO:
                    mode->loc = R3;
                    return RIGHT; // 向右转
                case L3:
                    mode->loc = L3_R;
                    return RIGHT; // 向右转
                case R3:
                    mode->loc = R3_R;
                    return RIGHT; // 向右转
            }
        }
        else if(mode->dir == FORWARD)
        {
            switch (mode->loc)
            {
                case ZERO:
                    mode->loc = ONE;
                    return FORWARD; // 向前进
                case ONE:
                    mode->loc = TWO;
                    return FORWARD; // 向前进
            }
        }
    }
    else if(mode->drug == RETURN_MODE)
    {
        switch( mode->loc )
        {
            case L1:
                mode->loc = ZERO;
                return LEFT; // 向左转
            case L2:
                mode->loc = ONE;
                return LEFT; // 向左转
            case R1:
                mode->loc = ZERO;
                return RIGHT; // 向右转
            case R2:
                mode->loc = ONE;
                return RIGHT; // 向右转
            case L3_L:
                mode->loc = L3;
                return LEFT; // 向左转
            case L3_R:
                mode->loc = L3;
                return RIGHT; // 向右转
            case R3_L:
                mode->loc = R3;
                return LEFT; // 向左转
            case R3_R:
                mode->loc = R3;
                return RIGHT; // 向右转
            case ONE:
                mode->loc = ZERO;
                return FORWARD; // 向前进
            case TWO:
                mode->loc = ONE;
                return FORWARD; // 向前进
            case L3:
                mode->loc = TWO;
                return RIGHT; // 向右转
            case R3:
                mode->loc = TWO;
                return LEFT; // 向左转
        }
    }
}