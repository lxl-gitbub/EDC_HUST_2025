//Ѳ��Ҫ���ڸ��Ѷ�Ѳ�ߵ�ͼ�����У�����Ҫ�޸ĵ����PID���ܴﵽ���õ�Ѳ��Ч��
//���������ʹ��˫��520L����������Ч��������ʹ�õĵ��PIDΪ��P:1.9,I:0.2,D:0.8
//IIC������·���ģ���޷�����PIDֵ�������Ҫʹ�õ��Դ�������ʹ�ô��ڵ�����ȥ�޸�PIDֵ
//���������ʹ��������PID��Ѳ��PID��Ч������û��˫��520�ĺã���Ҫ����ȥ���ڣ�

//Patrol to run on difficult patrol maps, it is necessary to modify the PID of the motor in order to achieve better patrol results
//This project is the use of two-wheel drive 520L chassis to adjust the effect of the motor PID used here: P: 1.9, I: 0.2, D: 0.8
//IIC drive four-way motor module can not change the PID value, so you need to use the computer serial port assistant to use the serial port command to modify the PID value
//! The rest of the chassis use this motor PID and patrol PID, the effect may not be as good as the 2WD 520L, you need to adjust yourself!

#include "AllHeader.h"

#define MOTOR_TYPE 5   //1:520��� 2:310��� 3:��������TT��� 4:TTֱ�����ٵ�� 5:L��520���
                       //1:520 motor 2:310 motor 3:speed code disc TT motor 4:TT DC reduction motor 5:L type 520 motor

int main(void)
{		
	
	//Ӳ����ʼ�� Hardware Initialization
	BSP_init();
    
    Set_Motor(MOTOR_TYPE);//���õ������  Setting motor parameters
    
	send_control_data(0,0,1); //����ֻ������ֵ������  Set to receive only numeric data
	
	while(1)
	{
		LineWalking(); //��ʼ��·Ѳ��  Starting eight-way patrols.
	}
}


