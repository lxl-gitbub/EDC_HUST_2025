#include "mode.h"

bool drugSet(MODE* mode)
{
    if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) == GPIO_PIN_RESET && mode->loc.n == 0)
    {
        HAL_Delay(10); // 防抖动延时
        if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) == GPIO_PIN_RESET) // 再次确认状态
        {
            mode->drug = PROPEL_MODE; // 送药模式
            mode->loc.n = 0; // 位置归零
            return true;
        }
    }
    else if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) == GPIO_PIN_SET && mode->loc.n != 0)
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

bool isEndOfWay(LOC l)
{
    if(l.n == 1)
    {
        if(l.trace[0] == LEFT || l.trace[0] == RIGHT)
        {
            return true; // 只有一个方向且是左或右，表示到达终点
        }
    }
    else if(l.n == 2)
    {
        if((l.trace[0] == FORWARD && l.trace[1] == LEFT) || 
           (l.trace[0] == FORWARD && l.trace[1] == RIGHT) )
        {
            return true; // 两个方向且是前进加左或右，表示到达终点
        }
    }
    else if (l.n == 4)
    {
        if((l.trace[0] == FORWARD && l.trace[1] == FORWARD) &&
           (l.trace[2] != FORWARD && l.trace[3] != FORWARD) &&
           (l.trace[2] != BACKWARD && l.trace[3] != BACKWARD))
        {
            return true; // 四个方向且前进两次，其他两次不是前进或后退，表示到达终点
        }
    }
    return false; // 其他情况不是终点
}

DIR ForToBack(DIR dir)
{
    // 将前进方向转换为返回方向
    switch(dir)
    {
        case LEFT:
            return RIGHT; // 左转变为右转
        case RIGHT:
            return LEFT; // 右转变为左转
        case FORWARD:
            return FORWARD; // 前进保持前进
        default:
            return dir; // 其他情况保持不变
    }
}

DIR DirGet(MODE* mode)
{
    // 获取下一个方向
   if(mode->drug == PROPEL_MODE)
    {
        mode->loc.trace[mode->loc.n++] = mode->dir; // 记录当前方向
        return mode->dir; // 返回当前方向
    }
    else if(mode->drug == RETURN_MODE)
    {
        if(isEndOfWay(mode->loc))
        {
            mode->loc.n --; // 减少记录的方向数
            return mode->loc.trace[mode->loc.n]; // 返回上一个方向
        }
        else
        {
            mode->loc.n --; // 减少记录的方向数
            return ForToBack(mode->loc.trace[mode->loc.n]); // 返回转换后的方向
        }
    }
		return FORWARD;
}

float DirToTheta(DIR dir)
{
    // 将方向转换为角度
    switch(dir)
    {
        case LEFT:
            return 90.0f; // 左转对应90度
        case RIGHT:
            return -90.0f; // 右转对应-90度
        case FORWARD:
            return 0.0f; // 前进对应0度
        case BACKWARD:
            return 180.0f; // 后退对应180度
        default:
            return 0.0f; // 默认返回0度
    }
}

float LocToTheta(LOC loc)
{
    float theta = 0.0f; // 初始化角度
    // 将位置转换为角度
    for(int i = 0; i < loc.n; i++)
    {
        theta += DirToTheta(loc.trace[i]); // 累加每个方向对应的角度
    }
    return theta;
}