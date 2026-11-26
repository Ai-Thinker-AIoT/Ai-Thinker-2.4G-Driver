#include "CI24R1.h"
#include "delay.h"

//按键及指示灯管脚定义
#define LED3		P16
#define LED4		P35
#define KEY1		P30
#define KEY2		P31

void main(void)
{
	KEY1 = 1;
	KEY2 = 1;
	CI24R1_Init();
	CI24R1_RX_Mode();
	while(1)
	{
	
		u8 buf[32] = {0};
		KEY1 = 1;
		KEY2 = 1;	
		
		/*用户根据需求进行延时处理，最低1ms的延时*/
		delay_ms(1);        
		/*--------------------------------------*/		
		
		if(!KEY1 || !KEY2)
		{
			delay_ms(10);
			if(!KEY1)
			{
			buf[0] = 0x55;
			CI24R1_TX_Mode();
			CI24R1_TxPacket(buf);
			delay_ms(200);
			}
			if(!KEY2)
			{
			buf[0] = 0xAA;
			CI24R1_TX_Mode();
			CI24R1_TxPacket(buf);
			delay_ms(200);
			}
			buf[0] = 0;
			CI24R1_RX_Mode();
		}	

		if(!CI24R1_RxPacket(buf))
		{
			switch(buf[0])
			{
				case 0x55:
				LED3 = 0;
				delay_ms(100);
				LED3 = 1;
				delay_ms(100);
				break;
				case 0xAA:
				LED4 = 0;
				delay_ms(100);
				LED4 = 1;
				delay_ms(100);
				break;
				default:
				break;
			}
			buf[0] = 0;	
		}
	}
}

