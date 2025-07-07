## 注意：

- 每次使用最好先进行校准（Z轴归零、加计校准），校准需要用上位机（MiniIMU)

- USART2开启，并开启中断

  **波特率为9600！**

- 引入JY61P库文件

## 代码

```c
/* USER CODE BEGIN PTD */
uint8_t GyroscopeUsart3RxBuffer[33];      //接收缓存
double GyroscopeChannelData[10];
uint8_t tempBuffer=0,RxBuffer;
/* USER CODE END PTD */

/* USER CODE BEGIN 2 */
//开启JY61P接收中断，注意波特率为9600
JY61P_Init(&huart2);
/* USER CODE END 2 */

//数据调用方式：(都为double型变量)
        //GyroscopeChannelData[0]	//X轴加速度（m/s*s）
        //GyroscopeChannelData[1]	//Y轴加速度
        //GyroscopeChannelData[2]	//Z轴加速度
        //GyroscopeChannelData[3]	//X轴角速度（°/s）
        //GyroscopeChannelData[4]	//Y轴角速度
        //GyroscopeChannelData[5]	//Z轴角速度            
        //GyroscopeChannelData[6]	//X轴(滚转角)角度（°）
        //GyroscopeChannelData[7]	//Y轴(俯仰角)角度
        //GyroscopeChannelData[8]	//Z轴(偏航角)角度
        //GyroscopeChannelData[9]	//实时温度（摄氏度）
```

