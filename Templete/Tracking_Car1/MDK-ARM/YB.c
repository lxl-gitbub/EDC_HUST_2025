#include <stdlib.h>
#include "stdio.h"
#include "string.h"

void send_control_data(u8 adjust,u8 aData,u8 dData)
{
	u8 send_buf[8] = "$0,0,0#";
	if(adjust == 1)//У׼����   Calibration command
	{
		send_buf[1] = '1';
	}
	else
	{
		send_buf[1] = '0';
	}
	if(aData == 1)//ģ��ֵ����   Analog data
	{
		send_buf[3] = '1';
		g_Amode_Data = 1;
	}
	else
	{
		send_buf[3] = '0';
		g_Amode_Data = 0;
	}
	if(dData == 1)//����ֵ����   Digital data
	{
		send_buf[5] = '1';
		g_Dmode_Data = 1;
	}
	else
	{
		send_buf[5] = '0';
		g_Dmode_Data = 0;
	}
	
	USART2_Send_ArrayU8(send_buf,strlen((char*)send_buf));
	
}

//�������� Data example��$D,x1:0,x2:0,x3:0,x4:0,x5:0,x6:0,x7:0,x8:0#
void Deal_Usart_Data(void) //��������������    Process digital data
{
	if(new_package[1]!='D')
	{
		return; //�˰����ݲ�������������   Not digital data
	}
	for(u8 i = 0;i<IR_Num;i++)
	{
		IR_Data_number[i] = (new_package[6+i*5]-'0');//���ַ�ת������  Convert char to number
	}
	
	memset(new_package,0,Package_size);//���������  Clear old data
  
}




//---------------------------ģ��ֵ������ص�   Analog value processing related---------------------------


//����������������ַ���(ָ������)  ԭʼ�ַ���  �ָ�����    Incoming parameters: reserved string (array of pointers) original string separator sign
void splitString(char* mystrArray[],char *str, const char *delimiter) 
{
    char *token = strtok(str, delimiter); //���ǵ�һ�ηָ�,���ᱣ����һ���ַ�ֵ  This is the first split, the first character value is not retained.
    int i =0;
    while (token != NULL) 
    {
        token = strtok(NULL, delimiter);
        mystrArray[i] = token;
        i++;
    }
}