#include "ti_msp_dl_config.h"
#include "delay.h"
#include "usart.h"
#include "bsp_motor_usart.h"
#include "app_irtracking.h"
#include "app_motor.h"
                    
#define MOTOR_TYPE 2   //1:520��� 2:310��� 3:��������TT��� 4:TTֱ�����ٵ�� 5:L��520���
                       //1:520 motor 2:310 motor 3:speed code disc TT motor 4:TT DC reduction motor 5:L type 520 motor

int main(void)
{
    USART_Init();//��ӡ���ڳ�ʼ�� Print serial port initialization
    printf("please wait...\r\n");

	 //ʹ��DMAͨ��  Enable DMA Channel
    NVIC_ClearPendingIRQ(UART_1_INST_INT_IRQN);
    DL_DMA_enableChannel(DMA, DMA_CH0_CHAN_ID);
    NVIC_EnableIRQ(UART_1_INST_INT_IRQN);
    
//    /*��·Ѳ��ģ���ʼ�� Initialization of the eight-way patrol module */
//    IRI2C_WriteByte(0x01,1);//���ƽ���У׼    Control Access Calibration
//    delay_ms(200);	
//    IRI2C_WriteByte(0x01,0);//�����˳�У׼    Control exit calibration
//    delay_ms(200);
    
    //���õ������    Set motor type
    Set_Motor(MOTOR_TYPE);
    
    //�޸ĵ��PID������Ĳ�����Ϊ����310�������õģ�����������Ҫ�Լ������޸�
    //Modify the motor PID, the parameters here are configured for the 4WD 310 chassis, other chassis need to test and modify their own!
	send_motor_PID(1.9,0.2,0.8);

    printf("Initialization Succeed\r\n");
    
	while(1)
	{
        LineWalking();//��ʼ��·Ѳ��  Starting eight-way patrols.
    }
	
}

