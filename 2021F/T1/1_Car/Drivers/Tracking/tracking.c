#include "tracking.h"
#include "AllHeader.h"

#define IRR_SPEED_HIGH  400  // 高速巡线速度
#define IRR_SPEED_LOW   200  // 低速巡线速度

// 高速PID参数
#define IRTrack_Trun_KP_HIGH 35
#define IRTrack_Trun_KI_HIGH 0
#define IRTrack_Trun_KD_HIGH 1

// 低速PID参数
#define IRTrack_Trun_KP_LOW  20
#define IRTrack_Trun_KI_LOW  0
#define IRTrack_Trun_KD_LOW  0.5

const float pid_out_max = 5000.0f; 
const float Integral_max = 500.0f; // 积分限幅值 
int pid_output_IRR = 0;
u8 trun_flag = 0;
static int8_t err = 0;
extern int Digtal[8];

// --- PID计算函数，参数可配置 ---
float PID_IR_Calc_Custom(int16_t actual_value, float kp, float ki, float kd)
{
    float pid_out = 0;
    int16_t error; 
    static int16_t error_last = 0; //上次的误差初始为0
    static float Integral = 0; // 初始化积分项

    error = actual_value;
    Integral += error;
    if (Integral > Integral_max) Integral = Integral_max;
    if (Integral < -Integral_max) Integral = -Integral_max;

    pid_out = error * kp + ki * Integral + (error - error_last) * kd;
    error_last = error;

    if (pid_out > pid_out_max) pid_out = pid_out_max;  
    if (pid_out < -pid_out_max) pid_out = -pid_out_max;

    return pid_out;
}

// --- 高速巡线 ---
void lineWalking_high(void)
{
    lineWalking_core(IRR_SPEED_HIGH, IRTrack_Trun_KP_HIGH, IRTrack_Trun_KI_HIGH, IRTrack_Trun_KD_HIGH);
}

// --- 低速巡线 ---
void lineWalking_low(void)
{
    lineWalking_core(IRR_SPEED_LOW, IRTrack_Trun_KP_LOW, IRTrack_Trun_KI_LOW, IRTrack_Trun_KD_LOW);
}

// --- 核心巡线逻辑，参数可配置 ---
void lineWalking_core(int16_t speed, float kp, float ki, float kd)
{
    static int8_t err = 0;
    static u8 x1,x2,x3,x4,x5,x6,x7,x8;

    x1 = Digtal[0];
    x2 = Digtal[1];
    x3 = Digtal[2];
    x4 = Digtal[3];
    x5 = Digtal[4];
    x6 = Digtal[5];
    x7 = Digtal[6];
    x8 = Digtal[7];

    if(x1 == 0 && x2 == 0  && x3 == 0&& x4 == 0 && x5 == 0 && x6 == 1  && x7 == 1 && x8 == 1)
    {
        err = -15;
        HAL_Delay(100);
    }
    else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 0 && x5 == 0 && x6 == 0  && x7 == 0 && x8 == 0)
    {
        err = 15;
        HAL_Delay(100);
    }
    else if(x1 == 0 &&  x2 == 0  && x3 == 0 && x6 == 0 && x7 == 0 && x8 == 0 )
    {
        err = 0;
        if(trun_flag == 1)
        {
            trun_flag = 0;
        }
    }
    else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 0 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 1)
    {
        err = -2;
    }
    else if(x1 == 1 && x2 == 1  && x3 == 0&& x4 == 0 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 1)
    {
        err = -4;
    }
    else if(x1 == 1 && x2 == 0  && x3 == 0&& x4 == 1 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 1)
    {
        err = -8;
    }
    else if(x1 == 0 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 1)
    {
        err = -10; 
    }
    else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 0 && x6 == 1  && x7 == 1 && x8 == 1)
    {
        err = 2;
    } 
    else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 0 && x6 == 0  && x7 == 1 && x8 == 1)
    {
        err = 4;
    }
    else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 1 && x6 == 0  && x7 == 0 && x8 == 1)
    {
        err = 8;
    }
    else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 0)
    {
        err = 10;
    }
    else if(x1 == 1 &&x2 == 1 &&x3 == 1 && x4 == 0 && x5 == 0 && x6 == 1 && x7 == 1&& x8 == 1)
    {
        err = 0;
    }
    // 其余情况保持上一个err

    pid_output_IRR = (int)(PID_IR_Calc_Custom(err*3, kp, ki, kd));  
    Motion_Car_Control(speed, 0, pid_output_IRR);
}

static float speed_lr = 0;
static float speed_fb = 0;
static float speed_spin = 0;
static int speed_L1_setup = 0;
static int speed_L2_setup = 0;
static int speed_R1_setup = 0;
static int speed_R2_setup = 0;


void Motion_Car_Control(int16_t V_x, int16_t V_y, int16_t V_z)
{
	float robot_APB = 115; //轮距的一半
	speed_lr = 0;
    speed_fb = V_x;
    speed_spin = (V_z / 1000.0f) * robot_APB;
    if (V_x == 0 && V_y == 0 && V_z == 0)
    {
        RSet(0);
				LSet(0);
        return;
    }

//    speed_L1_setup = speed_fb + speed_spin;
    speed_L2_setup = speed_fb + speed_spin;
//    speed_R1_setup = speed_fb  - speed_spin;
    speed_R2_setup = speed_fb  - speed_spin;
		
//    if (speed_L1_setup > 1000) speed_L1_setup = 1000;
//    if (speed_L1_setup < -1000) speed_L1_setup = -1000;
    if (speed_L2_setup > 1000) speed_L2_setup = 1000;
    if (speed_L2_setup < -1000) speed_L2_setup = -1000;
//    if (speed_R1_setup > 1000) speed_R1_setup = 1000;
//    if (speed_R1_setup < -1000) speed_R1_setup = -1000;
    if (speed_R2_setup > 1000) speed_R2_setup = 1000;
    if (speed_R2_setup < -1000) speed_R2_setup = -1000;
    
    //printf("%d\t,%d\t,%d\t,%d\r\n",speed_L1_setup,speed_L2_setup,speed_R1_setup,speed_R2_setup);
    
		LSet(speed_L2_setup);
		RSet(speed_R2_setup);
		
}

bool Road_detect(int nummin, int nummax)
{
		int sumIRDetect = 0;
		int i = 0;
		while(i < 8){	if(Digtal[i]==0) sumIRDetect ++; i++;}
		if(sumIRDetect <= nummax && sumIRDetect >= nummin) 	return 1;
		else return 0;
}
bool half_Detect(){
//三岔路口检测
	return Road_detect(5, 7);
}

bool cross_Roads_Detect(){
//十字路口（丁字路口）检测
	return Road_detect(5, 8);
}