#ifndef __IOI2C_H
#define __IOI2C_H

#include "AllHeader.h"

//IO��������	IO direction setting
#define SDA_IN()  {GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=8<<12;}
#define SDA_OUT() {GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=3<<12;}

//IO��������	 IO operation function
#define IIC_SCL    PBout(10) //SCL
#define IIC_SDA    PBout(11) //SDA	 
#define READ_SDA   PBin(11)  //����SDA 	Input SDA


//IIC���в�������	IIC all operation functions
int IIC_Start(void);				//����IIC��ʼ�ź�	Send IIC start signal
void IIC_Stop(void);	  			//����IICֹͣ�ź�	Send IIC stop signal
void IIC_Send_Byte(u8 txd);			//IIC����һ���ֽ�	IIC sends a byte
u8 IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�	IIC reads a byte
int IIC_Wait_Ack(void); 			//IIC�ȴ�ACK�ź�		IIC waits for ACK signal
void IIC_Ack(void);					//IIC����ACK�ź�		IIC sends ACK signal
void IIC_NAck(void);				//IIC������ACK�ź�	IIC does not send ACK signal

int i2cWrite(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *data);
int i2cRead(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf);

#endif

//------------------End of File----------------------------
