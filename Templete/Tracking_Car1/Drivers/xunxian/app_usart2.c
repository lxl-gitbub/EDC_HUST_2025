#include "app_usart2.h"
#include "usart.h"

#define Package_size 100


uint8_t rx_buff[Package_size];
uint8_t new_package[Package_size];
uint8_t g_new_package_flag = 0;//���յ��µ�һ�����ݱ�־

uint8_t IR_Data_number[IR_Num];//����ֵ
uint16_t IR_Data_Anglo[IR_Num];//ģ��ֵ
uint8_t g_Amode_Data = 0;//ģ���ͱ�־
uint8_t g_Dmode_Data = 0;//�����ͱ�־

void USART2_Send_U8(uint8_t ch)
{
  HAL_UART_Transmit(&huart2, &ch, 1, 0xFFFF);
}

// ����һ���ַ���
void USART2_Send_ArrayU8(uint8_t *BufferPtr, uint16_t Length)
{
  HAL_UART_Transmit(&huart2, BufferPtr, Length, 0xFFFF);
}

//Ѱ���ַ�������
int StringFind(const char *pSrc, const char *pDst)  
{  
    int i, j;  
    for (i=0; pSrc[i]!='\0'; i++)  
    {  
        if(pSrc[i]!=pDst[0])  
            continue;         
        j = 0;  
        while(pDst[j]!='\0' && pSrc[i+j]!='\0')  
        {  
            j++;  
            if(pDst[j]!=pSrc[i+j])  
            break;  
        }  
        if(pDst[j]=='\0')  
            return i;  
    }  
    return -1;  
}  

void Deal_IR_Usart(uint8_t rxtemp)
{
	static uint8_t g_start = 0;
  static uint8_t step = 0;
  if(rxtemp == '$')
  {  
    g_start = 1;//��ʼ���ձ�־
    rx_buff[step] = rxtemp;
    step++;
  }
  else
  {
    if(g_start == 0)
    {
      return;
    }
    else
    {
      rx_buff[step] = rxtemp;
      step ++;
      if(rxtemp == '#')//����
      {
        g_start = 0;
        step = 0;
        memcpy(new_package,rx_buff,Package_size);//��ֵ
        g_new_package_flag = 1;
        memset(rx_buff,0,Package_size);//�������

      }
      
      if(step >= Package_size)//�����쳣
      {
        g_start = 0;
        step = 0;
        memset(rx_buff,0,Package_size);//�������
      }
    }
  }
}


void send_control_data(uint8_t adjust,uint8_t aData,uint8_t dData)
{
	uint8_t send_buf[8] = "$0,0,0#";
	if(adjust == 1)//У׼����
	{
		send_buf[1] = '1';
	}
	else
	{
		send_buf[1] = '0';
	}
	if(aData == 1)//ģ��ֵ����
	{
		send_buf[3] = '1';
		g_Amode_Data = 1;
	}
	else
	{
		send_buf[3] = '0';
		g_Amode_Data = 0;
	}
	if(dData == 1)//����ֵ����
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

//�������ӣ�$D,x1:0,x2:0,x3:0,x4:0,x5:0,x6:0,x7:0,x8:0#
void Deal_Usart_Data(void) //��������������
{
	/*if(new_package[1]!='D')
	{
		return; //�˰����ݲ�������������
	}*/
	for(uint8_t i = 0;i<IR_Num;i++)
	{
		IR_Data_number[i] = (new_package[6+i*5]-'0');//���ַ�ת������ //6 11 16 21 26 
	}
	
	memset(new_package,0,Package_size);//���������
  
}




//---------------------------ģ��ֵ������ص�---------------------------


//����������������ַ���(ָ������)  ԭʼ�ַ���  �ָ�����
void splitString(char* mystrArray[],char *str, const char *delimiter) 
{
    char *token = strtok(str, delimiter); //���ǵ�һ�ηָ�,���ᱣ����һ���ַ�ֵ
    int i =0;
    while (token != NULL) 
    {
        token = strtok(NULL, delimiter);
        mystrArray[i] = token;
        i++;
    }
}

void Deal_Usart_AData(void) //����ģ��ֵ����
{
	//$A,x1:1000,x2:3450,x3:40,x4:450,x5:110,x6:4096,x7:780,x8:80#
	
	char* strArray[10];//ָ������ ���ȸ��ݷָ�Ŷ���  char 1�ֽ�   char* 4�ֽ�
	char* strArraytemp[2];
	char str_temp[Package_size] = {'\0'};
	char mystr_temp[8][10] = {'\0'}; //��ʱ����
	
	
	if(new_package[1]!='A')
	{
		return; //�˰����ݲ���ģ��ֵ����
	}
	strncpy(str_temp,(char*)new_package,strlen((char*)new_package)-1);
	
	splitString(strArray,str_temp, ", ");//�Զ����и�
	
	 //��������
    for (int i = 0; i < IR_Num; i++)
    {
    	  strcpy(mystr_temp[i],strArray[i]);
    	  splitString(strArraytemp,mystr_temp[i], ": ");
				IR_Data_Anglo[i] = atoi(strArraytemp[0]);
    }
		memset(new_package,0,Package_size);//���������
		
		memset(strArray,0,sizeof(strArray)); //����ָ����0
		memset(strArraytemp,0,sizeof(strArraytemp));//����ָ����0
  
}



