/* 需要在 HalDriverInit() 内 HalAdcInit() 内修改为 adcRef = HAL_ADC_REF_AIN7; */

#include "app.h"
#include "hal_adc.h"
#include "onboard.h"
#include "global.h"
#include "zcl_zigup.h"
#include <string.h>

#define LED1 P1_1
#define SWTICH P0_2

void sysInit(void)
{
    sysPinInit();
    sysAdcInit();
    sysTimInit();
    //sysUsartInit();
}

/* 初始化 */
void sysPinInit(void)
{
    P1DIR |= 0x02; // P1.1输出模式
    LED1 = 0;
    
    P0SEL &= ~0x04; // P0.2为普通IO
    P0DIR |= 0x04; // P0.2为输出模式
}

/* 初始化ADC */
void sysAdcInit(void)
{
    APCFG |= 0x10;
    P0SEL |= 0x10;
    P0DIR &= ~0x10;
}

/* 获取ADC值，转换为电压值，并返回 */
uint16 sysAdcGet(void)
{
    uint32 value;
    value = HalAdcRead(HAL_ADC_CHANNEL_4, HAL_ADC_RESOLUTION_14);
    value = value * 330;
    value = value / 8192;
    return (uint16)value;
}

/* 初始化定时器 */
void sysTimInit(void)
{
    CLKCONCMD &= ~0x40; // 设置系统时钟源为32MHZ晶振
    while (CLKCONSTA & 0x40)
        ;               // 等待晶振稳定为32M
    CLKCONCMD &= ~0x07; // 设置系统主时钟频率为32MHZ
    CLKCONCMD |= 0x38;  // 时钟速度32 MHz 定时器标记输出设置[5:3]250kHz

    PERCFG |= 0x40; // 定时器1的IO位置，选择备用位置2
    P2SEL &= ~0x10; // 定时器1优先

    P1DIR |= 0x02; // P1.1输出模式
    P1SEL |= 0x02; // timer1ͨ通道1映射到P1.1

    T1CC1H = 0x00; // 占空比
    T1CC1L = 0x00;

    T1CC0H = 0x00; // 周期
    T1CC0L = 0xff;

    T1CCTL1 = 0x1c; // 通道1比较模式
    T1CTL = 0x02;   //250KHz，不分频
}

/* 初始化串口 */
void sysUsartInit(void)
{
    PERCFG &= ~0x01;
    P0SEL = 0x3c;
    U0CSR |= 0x80;
    U0BAUD = 216;
    U0GCR = 10;
    U0UCR |= 0x80;
    UTX0IF = 0;
    EA = 1;
}

/* 发送一个字节 */
void sysSendByte(unsigned char c)
{
    U0DBUF = c;
    while (!UTX0IF)
        ;
    UTX0IF = 0;
}

/* 发送一个字符串 */
void sysSendString(unsigned char *str)
{
    while (*str != '\0')
    {
        sysSendByte(*str++);
    }
}

//uint8 rate[11]={0xF7,0xE1,0xC8,0xAF,0x96,0x7D,0x64,0x4B,0x32,0x19,0x0A};
void sysPwmSet(uint8 ucFlag)
{
    switch(ucFlag)
    {
    case LED_OFF:
          T1CC1H = 0x00; // 占空比
          T1CC1L = 0x00;
      break;
    case LED_ON:
          T1CC1H = 0x00; // 占空比
          T1CC1L = 0xAF;
      break;
    default:
      break;
    }
    sysAdcGet(); // 只有加上代码，PWM才会有输出，原因待分析
}

void ledSet(uint8 status)
{
	if (status)
	{
		//sysPwmSet(LED_ON);
                SWTICH = LED_ON;
	}
	else
	{
                //sysPwmSet(LED_OFF);
                SWTICH = LED_OFF;
	}
}

