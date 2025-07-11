#include "app_irtracking.h"

#define IRTrack_Trun_KP (15)
#define IRTrack_Trun_KI (0.2) 
#define IRTrack_Trun_KD (2) 
#define IRR_SPEED 			  200  //Ѳ���ٶ�   Patrol speed
// ����һ����ֵ�������жϴ��������ݱ仯�Ƿ����
#define CHANGE_THRESHOLD 3
const float pid_out_max = 5000.0f; 
const float Integral_max = 500.0f; // �����޷�ֵ 
int pid_output_IRR = 0;
u8 trun_flag = 0;
static int8_t err = 0;
// �洢��һ�εĴ������������
static uint8_t prev_sensor_data = 0;

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
void LineWalking(void)
{
    static int8_t err = 0;
    static u8 x1,x2,x3,x4,x5,x6,x7,x8;
    
    x1 = IR_Data_number[0];
    x2 = IR_Data_number[1];
    x3 = IR_Data_number[2];
    x4 = IR_Data_number[3];
    x5 = IR_Data_number[4];
    x6 = IR_Data_number[5];
    x7 = IR_Data_number[6];
    x8 = IR_Data_number[7];

    // ��������������ϳ�һ�� 8 λ�������� / Combine sensor data into an 8-bit binary number
    u8 sensor_pattern = (x1 << 7) | (x2 << 6) | (x3 << 5) | (x4 << 4) |
                        (x5 << 3) | (x6 << 2) | (x7 << 1) | x8;

    // ʹ�� switch-case ����ͬģʽ / Use switch-case to handle different patterns
    switch (sensor_pattern)
    {
        // err = -50 ����� / Cases for err = -50
        case 0b00000111: // 0000 0111
        case 0b00001111: // 0000 1111
        case 0b01111111: // 0111 1111
            err = -50;
            delay_ms(100);
            break;

        // err = -60 ����� / Cases for err = -60
        case 0b00011111: // 0001 1111
            err = -60;
            delay_ms(100);
            break;

        // err = 60 ����� / Cases for err = 60
        case 0b11100000: // 1110 0000
            err = 60;
            delay_ms(100);
            break;

        // err = 50 ����� / Cases for err = 50
        case 0b11110000: // 1111 0000
        case 0b11111000: // 1111 1000
        case 0b11111110: // 1111 1110
            err = 50;
            delay_ms(100);
            break;

        // err = 70 ����� / Cases for err = 70
        case 0b00010001:
        case 0b00001110:
        case 0b00011110:
            err = 70;
            delay_ms(100);
            break;

        // err = -70 ����� / Cases for err = -70
        case 0b01100000:
        case 0b01100001:
        case 0b01100010:
        case 0b01100011:
            err = -70;
            delay_ms(100);
            break;

        // err = 0 ����� / Cases for err = 0
        case 0b00000000: // ֱ�� / Straight
        case 0b11100111: // ֱ�� / Go straight
            err = 0;
            if (trun_flag == 1)
            {
                trun_flag = 0; // �ߵ�Ȧ�� / Walking in circles
            }
            break;

        // err = -1 ����� / Cases for err = -1
        case 0b11101111: // 1110 1111
            err = -1;
            break;

        // err = -2 ����� / Cases for err = -2
        case 0b11001111: // 1100 1111
        case 0b10011111: // 1001 1111
        case 0b00111111: // 0011 1111
            err = -2;
            break;

        // err = 1 ����� / Cases for err = 1
        case 0b11110111: // 1111 0111
            err = 1;
            break;

        // err = 2 ����� / Cases for err = 2
        case 0b11110011: // 1111 0011
        case 0b11111100: // 1111 1100
            err = 2;
            break;

        // err = 3 ����� / Cases for err = 3
        case 0b11111001: // 1111 1001
            err = 3;
            break;

        default:
            // ������һ��״̬ / Keep the previous state
            break;
    }

    // ���� PID ���������С���˶� / Calculate PID output and control car movement
    pid_output_IRR = (int)(PID_IR_Calc(err));
    Motion_Car_Control(IRR_SPEED, 0, pid_output_IRR);
}
//ɾ��ע��
//void LineWalking(void)
//{
//	static int8_t err = 0;
//	static u8 x1,x2,x3,x4,x5,x6,x7,x8;
//	
//	x1 = IR_Data_number[0];
//	x2 = IR_Data_number[1];
//	x3 = IR_Data_number[2];
//	x4 = IR_Data_number[3];
//	x5 = IR_Data_number[4];
//	x6 = IR_Data_number[5];
//	x7 = IR_Data_number[6];
//	x8 = IR_Data_number[7];
//	


////�����ж��Ƿ�ֱ�ǻ����  Prioritize whether to right angles or acute angles
//	 if(x1 == 0 && x2 == 0  && x3 == 0&& x4 == 0 && x5 == 0 && x6 == 1  && x7 == 1 && x8 == 1) // 0000 0111
//	{
//		err = -50;
//        delay_ms(100);
//	}
//	
//	//����� д��������Կ�����Ч
//	else if(x1==0 && x8 ==0 && x4 ==1 && x5 ==1)
//{
//    err = 70;  // ����ת�����
//    delay_ms(100);  // ͳһ�ӳ�ʱ��
//}

//	else if(x1 == 0 && x2 == 0  && x3 == 0&& x4 == 0 && x5 ==1  && x6 == 1  && x7 == 1 && x8 == 1) // 0000 1111
//	{
//		err = -50;
//        delay_ms(100);
//	}

//		else if(x1 == 0 && x2 == 0  && x3 == 0&& x4 == 1 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 1) // 00011111
//	{
//		err = -60;
//        delay_ms(100);
//	}
//    else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 0 && x5 == 0 && x6 == 0  && x7 == 0 && x8 == 0) // 1110 0000
//	{
//		err = 60;
//        delay_ms(100);
//	}
//	    else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 0 && x6 == 0  && x7 == 0 && x8 == 0) // 1111 0000
//	{
//		err = 50;
//        delay_ms(100);
//	}
//		 else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 1 && x6 == 0  && x7 == 0 && x8 == 0) // 1111 1000
//	{
//		err = 50;
//        delay_ms(100);
//	}
//	
//	else if((x1 == 0 && x2 == 1 && x3 == 1) && (x6 == 0 || x7 == 0 || x8 == 0))
//{
//    err = -70;  // ����ת�����
//    delay_ms(100);  // ͳһ�ӳ�ʱ��
//}
//// �Ż����������ж�

//	else if((x8 == 0 && x4 == 0 && x5 == 1)||(x8 == 0 && x7==0&& x1==0 &&x2==0&& x4 == 1 && x5 == 1)||((x8 == 0 && x7 == 1 && x6 == 1) && (x1 == 0 || x2 == 0 || x3 == 0)))
//{
//    err = 70;  // ����ת�����
//    delay_ms(100);  // ͳһ�ӳ�ʱ��
//}

//  else if(x1 == 0 &&  x2 == 0  && x7 == 0 && x8 == 0 ) //���߶�����ֱ��    Both sides are lit. Run straight.
//	{
//		err = 0;
//		if(trun_flag == 1)
//		{
//			trun_flag = 0;//�ߵ�Ȧ��    Walking in circles.
//		}
//	}
//	

//	else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 0 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 1) // 1110 1111
//	{
//		err = -1;
//	}
//	else if(x1 == 1 && x2 == 1  && x3 == 0&& x4 == 0 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 1) // 1100 1111
//	{
//		err = -2;
//	}

//	else if(x1 == 1 && x2 == 0  && x3 == 0&& x4 == 1 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 1) // 1001 1111
//	{
//		err = -2;
//	}
//    
//		else if(x1 == 0 && x2 == 0  && x3 == 1&& x4 == 1 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 1) // 0011 1111
//	{
//		err = -2;  
//	}
//	else if(x1 == 0 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 1) // 0111 1111
//	{
//		err = -50; 
//	}

//	
//	else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 0 && x6 == 1  && x7 == 1 && x8 == 1) // 1111 0111
//	{
//		err = 1;
//	} 
//	else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 0 && x6 == 0  && x7 == 1 && x8 == 1) // 1111 0011
//	{
//		err = 2;
//	}

//	else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 1 && x6 == 0  && x7 == 0 && x8 == 1) // 1111 1001
//	{
//		err = 3;
//	}
//	

//	else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 1 && x6 == 1  && x7 == 0 && x8 == 0) // 1111 1100
//	{
//		err = 2; 
//	}
//		else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 0) // 1111 1110
//	{
//		err = 50;
//	}
//	

//	
// 
//	else if(x1 == 1 &&x2 == 1 &&x3 == 1 && x4 == 0 && x5 == 0 && x6 == 1 && x7 == 1&& x8 == 1) //ֱ�� go straight
//	{
//		err = 0;
//	}
//    
//	//ʣ�µľͱ�����һ��״̬	    The rest will stay the same.
//    
//    
//	pid_output_IRR = (int)(PID_IR_Calc(err));

//	Motion_Car_Control(IRR_SPEED, 0, pid_output_IRR);	

//}








//ע��δɾ����
//void LineWalking(void)
//{
//	static int8_t err = 0;
//	static u8 x1,x2,x3,x4,x5,x6,x7,x8;
//	
//	x1 = IR_Data_number[0];
//	x2 = IR_Data_number[1];
//	x3 = IR_Data_number[2];
//	x4 = IR_Data_number[3];
//	x5 = IR_Data_number[4];
//	x6 = IR_Data_number[5];
//	x7 = IR_Data_number[6];
//	x8 = IR_Data_number[7];
//	


////�����ж��Ƿ�ֱ�ǻ����  Prioritize whether to right angles or acute angles
//	 if(x1 == 0 && x2 == 0  && x3 == 0&& x4 == 0 && x5 == 0 && x6 == 1  && x7 == 1 && x8 == 1) // 0000 0111
//	{
//		err = -50;
//        delay_ms(100);
//	}
//	
//	//����� д��������Կ�����Ч
//	else if(x1==0 && x8 ==0 && x4 ==1 && x5 ==1)
//{
//    err = 70;  // ����ת�����
//    delay_ms(100);  // ͳһ�ӳ�ʱ��
//}

//	else if(x1 == 0 && x2 == 0  && x3 == 0&& x4 == 0 && x5 ==1  && x6 == 1  && x7 == 1 && x8 == 1) // 0000 1111
//	{
//		err = -50;
//        delay_ms(100);
//	}

//		else if(x1 == 0 && x2 == 0  && x3 == 0&& x4 == 1 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 1) // 00011111
//	{
//		err = -60;
//        delay_ms(100);
//	}
//    else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 0 && x5 == 0 && x6 == 0  && x7 == 0 && x8 == 0) // 1110 0000
//	{
//		err = 60;
//        delay_ms(100);
//	}
//	    else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 1 && x1 == 0 && x6 == 0  && x7 == 0 && x8 == 0) // 1111 0000
//	{
//		err = 50;
//        delay_ms(100);
//	}
//		 else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 1 && x6 == 0  && x7 == 0 && x8 == 0) // 1111 1000
//	{
//		err = 50;
//        delay_ms(100);
//	}
////����������
////	else if( x1==0&&x2==1&&(x6 == 0 || x7 == 0))
////	{
////	err = -65;
////        delay_ms(150);
////	}
////		else if( x8==0&&x7==1&&(x2== 0 || x3== 0))
////	{
////	err = 65;
////        delay_ms(150);
////	}
////			
//	else if((x1 == 0 && x2 == 1 && x3 == 1) && (x6 == 0 || x7 == 0 || x8 == 0))
//{
//    err = -70;  // ����ת�����
//    delay_ms(100);  // ͳһ�ӳ�ʱ��
//}
//// �Ż����������ж�

//	else if((x8 == 0 && x4 == 0 && x5 == 1)||(x8 == 0 && x7==0&& x1==0 &&x2==0&& x4 == 1 && x5 == 1)||((x8 == 0 && x7 == 1 && x6 == 1) && (x1 == 0 || x2 == 0 || x3 == 0)))
//{
//    err = 70;  // ����ת�����
//    delay_ms(100);  // ͳһ�ӳ�ʱ��
//}

//  else if(x1 == 0 &&  x2 == 0  && x7 == 0 && x8 == 0 ) //���߶�����ֱ��    Both sides are lit. Run straight.
//	{
//		err = 0;
//		if(trun_flag == 1)
//		{
//			trun_flag = 0;//�ߵ�Ȧ��    Walking in circles.
//		}
//	}
//	

//	else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 0 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 1) // 1110 1111
//	{
//		err = -1;
//	}
//	else if(x1 == 1 && x2 == 1  && x3 == 0&& x4 == 0 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 1) // 1100 1111
//	{
//		err = -2;
//	}
////	else if(x1 == 1 && x2 == 1  && x3 == 0&& x4 == 1 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 1) // 1101 1111
////	{
////		err = -2;
////	}
//	
////	else if(x1 == 1 && x2 == 0  && x3 == 1&& x4 == 1 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 1) // 1011 1111
////	{
////		err = -3;
////	}
//	else if(x1 == 1 && x2 == 0  && x3 == 0&& x4 == 1 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 1) // 1001 1111
//	{
//		err = -2;
//	}
//    
//		else if(x1 == 0 && x2 == 0  && x3 == 1&& x4 == 1 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 1) // 0011 1111
//	{
//		err = -2;  
//	}
//	else if(x1 == 0 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 1) // 0111 1111
//	{
//		err = -50; 
//	}

//	
//	else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 0 && x6 == 1  && x7 == 1 && x8 == 1) // 1111 0111
//	{
//		err = 1;
//	} 
//	else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 0 && x6 == 0  && x7 == 1 && x8 == 1) // 1111 0011
//	{
//		err = 2;
//	}
////	else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 1 && x6 == 0  && x7 == 1 && x8 == 1) // 1111 1011
////	{
////		err = 2;
////	}
//	else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 1 && x6 == 0  && x7 == 0 && x8 == 1) // 1111 1001
//	{
//		err = 3;
//	}
//	
////	else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 1 && x6 == 1  && x7 == 0 && x8 == 1) // 1111 1101
////	{
////		err = 3;
////	}
//	else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 1 && x6 == 1  && x7 == 0 && x8 == 0) // 1111 1100
//	{
//		err = 2; 
//	}
//		else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 0) // 1111 1110
//	{
//		err = 50;
//	}
//	

//	
// 
//	else if(x1 == 1 &&x2 == 1 &&x3 == 1 && x4 == 0 && x5 == 0 && x6 == 1 && x7 == 1&& x8 == 1) //ֱ�� go straight
//	{
//		err = 0;
//	}
//    
//	//ʣ�µľͱ�����һ��״̬	    The rest will stay the same.
//    
//    
//	pid_output_IRR = (int)(PID_IR_Calc(err));

//	Motion_Car_Control(IRR_SPEED, 0, pid_output_IRR);	

//}

