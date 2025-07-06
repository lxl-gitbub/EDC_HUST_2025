/**
 ****************************************************************************************************
 * @file        demo.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2022-06-21
 * @brief       ATK-MS601Mģ�����ʵ��
 * @license     Copyright (c) 2020-2032, ������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� ������ H743������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#include "demo.h"
#include "./BSP/ATK_MS601M/atk_ms601m.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/key/key.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"

/**
 * @brief       LCD UI��ʼ��
 * @param       ��
 * @retval      ��
 */
static void demo_lcd_ui_init(void)
{
    lcd_show_string(20, 130, 200, 16, 16, "Roll :   0.00'", BLUE);
    lcd_show_string(20, 151, 200, 16, 16, "Pitch:   0.00'", BLUE);
    lcd_show_string(20, 172, 200, 16, 16, "Yaw  :   0.00'", BLUE);
}

/**
 * @brief       LCD��ʾ���������Ϣ
 * @param       ��
 * @retval      ��
 */
static void demo_show_data_lcd(atk_ms601m_attitude_data_t *attitude_dat)
{
    int32_t tmp;
    
    /* ����� */
    tmp = attitude_dat->roll * 100;
    if (tmp < 0)
    {
        tmp = -tmp;
        lcd_show_string(68, 130, 8, 16, 16, "-", BLUE);
    }
    else
    {
        lcd_show_string(68, 130, 8, 16, 16, " ", BLUE);
    }
    lcd_show_num(76, 130, tmp / 100, 3, 16, BLUE);
    lcd_show_xnum(108, 130, tmp % 100, 2, 16, 0x80, BLUE);
    
    /* ������ */
    tmp = attitude_dat->pitch * 100;
    if (tmp < 0)
    {
        tmp = -tmp;
        lcd_show_string(68, 151, 8, 16, 16, "-", BLUE);
    }
    else
    {
        lcd_show_string(68, 151, 8, 16, 16, " ", BLUE);
    }
    lcd_show_num(76, 151, tmp / 100, 3, 16, BLUE);
    lcd_show_xnum(108, 151, tmp % 100, 2, 16, 0x80, BLUE);
    
    /* ����� */
    tmp = attitude_dat->yaw * 100;
    if (tmp < 0)
    {
        tmp = -tmp;
        lcd_show_string(68, 172, 8, 16, 16, "-", BLUE);
    }
    else
    {
        lcd_show_string(68, 172, 8, 16, 16, " ", BLUE);
    }
    lcd_show_num(76, 172, tmp / 100, 3, 16, BLUE);
    lcd_show_xnum(108, 172, tmp % 100, 2, 16, 0x80, BLUE);
}

/**
 * @brief       ����0���ܣ���ȡ����ʾATK-MS601M����
 * @param       ��
 * @retval      ��
 */
static void demo_key0_fun(void)
{
    atk_ms601m_attitude_data_t attitude_dat;           /* ��̬������ */
    atk_ms601m_gyro_data_t gyro_dat;                   /* ���������� */
    atk_ms601m_accelerometer_data_t accelerometer_dat; /* ���ٶȼ����� */
    
    /* ��ȡATK-MS901���� */
    atk_ms601m_get_attitude(&attitude_dat, 100);                            /* ��ȡ��̬������ */
    atk_ms601m_get_gyro_accelerometer(&gyro_dat, &accelerometer_dat, 100);  /* ��ȡ�����ǡ����ٶȼ����� */
    
    /* LCD��ʾ���� */
    demo_show_data_lcd(&attitude_dat);
    
    /* ���ڴ�ӡ���� */
    printf("Roll: %.02f�� Pitch: %.02f�� Yaw: %.02f��\r\n", attitude_dat.roll, attitude_dat.pitch, attitude_dat.yaw);
    printf("Gx: %.02f��/s Gy: %.02f��/s Gz: %.02f��/s\r\n", gyro_dat.x, gyro_dat.y, gyro_dat.z);
    printf("Ax: %.02fG Ay: %.02fG Az: %.02fG\r\n", accelerometer_dat.x, accelerometer_dat.y, accelerometer_dat.z);
    printf("****************************************\r\n\r\n");
}

/**
 * @brief       ������ʾ��ں���
 * @param       ��
 * @retval      ��
 */
void demo_run(void)
{
    uint8_t ret;
    uint8_t key;
    
    /* ��ʼ��ATK-MS601M */
    ret = atk_ms601m_init(115200);
    if (ret != 0)
    {
        printf("ATK-MS601M init failed!\r\n");
        while (1)
        {
            LED0_TOGGLE();
            delay_ms(200);
        }
    }
    
    printf("ATK-MS601M init success!\r\n\n");
    
    /* LCD UI��ʼ�� */
    demo_lcd_ui_init();
    
    while (1)
    {
        key = key_scan(0);
        
        switch (key)
        {
            case KEY0_PRES:
            {
                /* ��ȡ����ʾATK-MS901���� */
                demo_key0_fun();
                break;
            }
            default:
            {
                break;
            }
        }
        
        delay_ms(10);
    }
}
