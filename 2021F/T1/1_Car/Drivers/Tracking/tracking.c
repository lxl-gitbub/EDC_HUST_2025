#include "tracking.h"
#include "AllHeader.h"

#define IRTrack_Trun_KP (10)//ԭ��Ϊ30
#define IRTrack_Trun_KI (0) 
#define IRTrack_Trun_KD (0) 
#define IRR_SPEED 			  300  //Ѳ���ٶ�   Patrol speed
// ����һ����ֵ�������жϴ��������ݱ仯�Ƿ����
#define CHANGE_THRESHOLD 3

const float pid_out_max = 5000.0f; 
const float Integral_max = 500.0f; // �����޷�ֵ 
int pid_output_IRR = 0;
u8 trun_flag = 0;
static int8_t err = 0;
extern int Digtal[8];
// �洢��һ�εĴ������������
static uint8_t prev_sensor_data = 0;

void Motion_Car_Control(int16_t V_x, int16_t V_y, int16_t V_z);

float PID_IR_Calc(int16_t actual_value)
{
    float pid_out = 0;
    int16_t error; 
    static int16_t error_last = 0; //�ϴε�����ʼΪ0  Last error
    static float Integral = 0; // ��ʼ�������� Initialize integral term

    error = actual_value;
//    if(err == 0)          
//    {
//        Integral = 0;          //��������   Integral cleared
//    }
    Integral += error;           // ���»�����������޷� Update the integral term and limit it
    if (Integral > Integral_max) Integral = Integral_max;               //�����޷� Integral limiting
    if (Integral < -Integral_max) Integral = -Integral_max;             //�����޷� Integral limiting

    // λ��ʽ PID
    pid_out = error * IRTrack_Trun_KP
              + IRTrack_Trun_KI * Integral
              + (error - error_last) * IRTrack_Trun_KD;

    error_last = error;       // ���»�����������޷� Update the integral term and limit it

    // ����������޷�Output limiting value
    if (pid_out > pid_out_max) pid_out = pid_out_max;  
    if (pid_out < -pid_out_max) pid_out = -pid_out_max;

    return pid_out;
}

void lineWalking(){
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
	
	
	 if(x1 == 0 && x2 == 0  && x3 == 0&& x4 == 0 && x5 == 0 && x6 == 1  && x7 == 1 && x8 == 1) // 0000 0111
	{
		err = -15;
        HAL_Delay(100);
	}
    else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 0 && x5 == 0 && x6 == 0  && x7 == 0 && x8 == 0) // 1110 0000
	{
		err = 15;
        HAL_Delay(100);
	}

  else if(x1 == 0 &&  x2 == 0  && x3 == 0 && x6 == 0 && x7 == 0 && x8 == 0 ) //���߶�����ֱ��    Both sides are lit. Run straight.
	{
		err = 0;
		if(trun_flag == 1)
		{
			trun_flag = 0;//�ߵ�Ȧ��    Walking in circles.
		}
	}
	
// else if(x1 == 0 &&  x3 == 0 && x4 == 0 && x5 == 0 && x8 == 0 )
//	{
//		err = 0;
//	}
//	//���ֱ��  Add Right Angle
//	else if((x1 == 0 || x2 == 0 ) && x8 == 1) 
//	{
//		err = -15; 
//	}
//	//���ֱ��  Add Right Angle
//	else if((x7 == 0 ||  x8 == 0) && x1 == 1) 
//	{
//		err = 15 ;
//	}
	


	else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 0 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 1) // 1110 1111
	{
		err = -2;
	}
	else if(x1 == 1 && x2 == 1  && x3 == 0&& x4 == 0 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 1) // 1100 1111
	{
		err = -4;
	}
//	else if(x1 == 1 && x2 == 1  && x3 == 0&& x4 == 1 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 1) // 1101 1111
//	{
//		err = -2;
//	}
	
//	else if(x1 == 1 && x2 == 0  && x3 == 1&& x4 == 1 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 1) // 1011 1111
//	{
//		err = -3;
//	}
	else if(x1 == 1 && x2 == 0  && x3 == 0&& x4 == 1 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 1) // 1001 1111
	{
		err = -8;
	}
    
//		else if(x1 == 0 && x2 == 0  && x3 == 1&& x4 == 1 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 1) // 0011 1111
//	{
//		err = -4;   //ע�ͣ�����ֱ�Ǵ��� Note, when treated as a right angle
//	}
	else if(x1 == 0 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 1) // 0111 1111
	{
		err = -10; 
	}

	

	
	
	
	else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 0 && x6 == 1  && x7 == 1 && x8 == 1) // 1111 0111
	{
		err = 2;
	} 
	else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 0 && x6 == 0  && x7 == 1 && x8 == 1) // 1111 0011
	{
		err = 4;
	}
//	else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 1 && x6 == 0  && x7 == 1 && x8 == 1) // 1111 1011
//	{
//		err = 2;
//	}
	else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 1 && x6 == 0  && x7 == 0 && x8 == 1) // 1111 1001
	{
		err = 8;
	}
	
//	else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 1 && x6 == 1  && x7 == 0 && x8 == 1) // 1111 1101
//	{
//		err = 3;
//	}
//	else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 1 && x6 == 1  && x7 == 0 && x8 == 0) // 1111 1100
//	{
//		err = 4; ///����ֱ�Ǵ���  treat as a right angle
//	}
		else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 0) // 1111 1110
	{
		err = 10;
	}
	

	
 
	else if(x1 == 1 &&x2 == 1 &&x3 == 1 && x4 == 0 && x5 == 0 && x6 == 1 && x7 == 1&& x8 == 1) //ֱ�� go straight
	{
		err = 0;
	}
	//ʣ�µľͱ�����һ��״̬	    The rest will stay the same.
    
  pid_output_IRR = (int)(PID_IR_Calc(   err*3   ));  
	Motion_Car_Control(IRR_SPEED, 0, pid_output_IRR);
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
	float robot_APB = 110; //�־��һ��
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
//����·�ڼ��
	return Road_detect(5, 7);
}

bool cross_Roads_Detect(){
//ʮ��·�ڣ�����·�ڣ����
	return Road_detect(5, 8);
}