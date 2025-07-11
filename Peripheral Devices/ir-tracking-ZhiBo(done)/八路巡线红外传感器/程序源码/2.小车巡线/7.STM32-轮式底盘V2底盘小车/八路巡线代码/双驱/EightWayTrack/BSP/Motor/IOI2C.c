#include "ioi2c.h"

/**************************************************************************
Function: Simulate IIC start signal
Input   : none
Output  : 1
�������ܣ�ģ��IIC��ʼ�ź�
��ڲ�������
����  ֵ��1
**************************************************************************/
int IIC_Start(void)
{
	SDA_OUT();     //sda�����	sda line output
	IIC_SDA=1;
	if(!READ_SDA)return 0;	
	IIC_SCL=1;
	delay_us(1);
 	IIC_SDA=0; //START:when CLK is high,DATA change form high to low 
	if(READ_SDA)return 0;
	delay_us(1);
	IIC_SCL=0;//ǯסI2C���ߣ�׼�����ͻ�������� 	Clamp the I2C bus and prepare to send or receive data
	return 1;
}

/**************************************************************************
Function: Analog IIC end signal
Input   : none
Output  : none
�������ܣ�ģ��IIC�����ź�
��ڲ�������
����  ֵ����
**************************************************************************/	  
void IIC_Stop(void)
{
	SDA_OUT();//sda�����	sda line output
	IIC_SCL=0;
	IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	delay_us(1);
	IIC_SCL=1; 
	IIC_SDA=1;//����I2C���߽����ź�	Send I2C bus end signal
	delay_us(1);							   	
}

/**************************************************************************
Function: IIC wait the response signal
Input   : none
Output  : 0��No response received��1��Response received
�������ܣ�IIC�ȴ�Ӧ���ź�
��ڲ�������
����  ֵ��0��û���յ�Ӧ��1���յ�Ӧ��
**************************************************************************/
int IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN();      //SDA����Ϊ����  SDA is set as input
	IIC_SDA=1;
	delay_us(1);	   
	IIC_SCL=1;
	delay_us(1);	 
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>50)
		{
			IIC_Stop();
			return 0;
		}
	  delay_us(1);
	}
	IIC_SCL=0;//ʱ�����0 	 Clock output 0  
	return 1;  
} 

/**************************************************************************
Function: IIC response
Input   : none
Output  : none
�������ܣ�IICӦ��
��ڲ�������
����  ֵ����
**************************************************************************/
void IIC_Ack(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=0;
	delay_us(1);
	IIC_SCL=1;
	delay_us(1);
	IIC_SCL=0;
}
	
/**************************************************************************
Function: IIC don't reply
Input   : none
Output  : none
�������ܣ�IIC��Ӧ��
��ڲ�������
����  ֵ����
**************************************************************************/    
void IIC_NAck(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=1;
	delay_us(1);
	IIC_SCL=1;
	delay_us(1);
	IIC_SCL=0;
}
/**************************************************************************
Function: IIC sends a byte
Input   : txd��Byte data sent
Output  : none
�������ܣ�IIC����һ���ֽ�
��ڲ�����txd�����͵��ֽ�����
����  ֵ����
**************************************************************************/	  
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	  SDA_OUT(); 	    
    IIC_SCL=0;//����ʱ�ӿ�ʼ���ݴ���	Pull the clock low to start data transmission
    for(t=0;t<8;t++)
    {              
			IIC_SDA=(txd&0x80)>>7;
			txd<<=1; 	  
			delay_us(1);   
			IIC_SCL=1;
			delay_us(1); 
			IIC_SCL=0;	
			delay_us(1);
    }	 
} 	 
  
/**************************************************************************
Function: IIC write data to register
Input   : addr��Device address��reg��Register address��len;Number of bytes��data��Data
Output  : 0��Write successfully��1��Failed to write
�������ܣ�IICд���ݵ��Ĵ���
��ڲ�����addr���豸��ַ��reg���Ĵ�����ַ��len;�ֽ�����data������
����  ֵ��0���ɹ�д�룻1��û�гɹ�д��
**************************************************************************/
int i2cWrite(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *data)
{
		int i;
    if (!IIC_Start())
        return 1;
    IIC_Send_Byte(addr << 1 );
    if (!IIC_Wait_Ack()) {
        IIC_Stop();
        return 1;
    }
    IIC_Send_Byte(reg);
    IIC_Wait_Ack();
		for (i = 0; i < len; i++) {
        IIC_Send_Byte(data[i]);
        if (!IIC_Wait_Ack()) {
            IIC_Stop();
            return 0;
        }
    }
    IIC_Stop();
    return 0;
}
/**************************************************************************
Function: IIC read register data
Input   : addr��Device address��reg��Register address��len;Number of bytes��*buf��Data read out
Output  : 0��Read successfully��1��Failed to read
�������ܣ�IIC���Ĵ���������
��ڲ�����addr���豸��ַ��reg���Ĵ�����ַ��len;�ֽ�����*buf���������ݻ���
����  ֵ��0���ɹ�������1��û�гɹ�����
**************************************************************************/

int i2cRead(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf)
{
    if (!IIC_Start())
        return 1;
    IIC_Send_Byte(addr << 1);
    if (!IIC_Wait_Ack()) {
        IIC_Stop();
        return 1;
    }
    IIC_Send_Byte(reg);
    IIC_Wait_Ack();
    IIC_Start();
    IIC_Send_Byte((addr << 1)+1);
    IIC_Wait_Ack();
    while (len) {
        if (len == 1)
            *buf = IIC_Read_Byte(0);
        else
            *buf = IIC_Read_Byte(1);
        buf++;
        len--;
    }
    IIC_Stop();
    return 0;
}

/**************************************************************************
Function: IIC reads a byte
Input   : ack��Send response signal or not��1��Send��0��Do not send
Output  : receive��Data read
�������ܣ�IIC��ȡһ��λ
��ڲ�����ack���Ƿ���Ӧ���źţ�1�����ͣ�0��������
����  ֵ��receive����ȡ������
**************************************************************************/ 
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA����Ϊ����	SDA is set as input
    for(i=0;i<8;i++ )
	 {
			IIC_SCL=0; 
			delay_us(2);
			IIC_SCL=1;
			receive<<=1;
			if(READ_SDA)receive++;   
			delay_us(2); 
    }					 
    if (ack)
        IIC_Ack(); //����ACK 	Send ACK
    else
        IIC_NAck();//����nACK 	Send nACK 
    return receive;
}
