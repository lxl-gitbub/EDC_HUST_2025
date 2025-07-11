#include "stm32f10x.h"
#include "bsp_usart.h"
#include "delay.h"


extern u8 g_new_package_flag;

int main()
{
	
	SystemInit();
	delay_init();
	
	uart_init(115200);
	USART2_init(115200);
	
	delay_ms(1000);//�ȴ���������
	send_control_data(0,0,1); //����Э�鷢�Ͷ�Ӧ������� ��Ϊֻ������ֵ������
	
	while(1)
	{
		if(g_new_package_flag == 1)
		{
			g_new_package_flag = 0;
			
			if(g_Dmode_Data == 1)
			{
				Deal_Usart_Data();//��ֵ�����ݴ���
				printf("x1:%d,x2:%d,x3:%d,x4:%d,x5:%d,x6:%d,x7:%d,x8:%d\r\n",IR_Data_number[0],IR_Data_number[1],IR_Data_number[2],IR_Data_number[3],IR_Data_number[4],IR_Data_number[5],IR_Data_number[6],IR_Data_number[7]);
			}
			if(g_Amode_Data == 1)
			{
				Deal_Usart_AData();//ģ�������ݴ���
				printf("x1:%d,x2:%d,x3:%d,x4:%d,x5:%d,x6:%d,x7:%d,x8:%d\r\n",IR_Data_Anglo[0],IR_Data_Anglo[1],IR_Data_Anglo[2],IR_Data_Anglo[3],IR_Data_Anglo[4],IR_Data_Anglo[5],IR_Data_Anglo[6],IR_Data_Anglo[7]);
			
			}
			
		}
		
		
	}
}

