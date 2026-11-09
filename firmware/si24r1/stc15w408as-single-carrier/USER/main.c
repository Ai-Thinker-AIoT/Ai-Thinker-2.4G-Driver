#include "SI24R1.h"
#include "delay.h"
//按键及指示灯管脚定义
#define LED3		P16
#define LED4		P35
#define KEY1		P30
#define KEY2		P31

void main(void)
{
	u8 buf[32] = {0};
	KEY1 = 1;
	KEY2 = 1;
	SI24R1_Init();

#if (SI24R1_TEST_MODE == 1)
	// 恒载波模式：未调制单载波，CE 常高。用于载波功率/频率容限/杂散测试。
	// 注意：单载波没有占用带宽，此模式下的 OBW 只有几十 kHz。
	SI24R1_CW_Mode();
	while(1);                                     // 载波持续输出
#elif (SI24R1_TEST_MODE == 2)
	// 调制波模式：2 Mbps 连续调制载波，+7 dBm，2475 MHz。
	// 判据是 occupied bandwidth（含 99% 总平均功率的带宽），下限 500 kHz。
	// 205 kbps 档实测 99% OBW 仅 433 kHz -> 不达标；1 Mbps 约 1 MHz；
	// 2 Mbps 约 2 MHz -> 余量最大。
	// CE 常高 + TX FIFO 持续喂伪随机载荷，载波不间断；载荷由函数内部生成，
	// 这里不用预置。用于占用带宽 / 6dB 带宽 / 功率谱密度测试。
	SI24R1_Continuous_Tx_Test(buf, 0);
#else
	SI24R1_RX_Mode();
	while(1)
	{
		KEY1 = 1;
		KEY2 = 1;
		if(!KEY1 || !KEY2)
		{
			delay_ms(10);
			if(!KEY1)
			{
				buf[0] = 0x55;
				SI24R1_TX_Mode();
				SI24R1_TxPacket(buf);
				delay_ms(200);
			}
			if(!KEY2)
			{
				buf[0] = 0xAA;
				SI24R1_TX_Mode();
				SI24R1_TxPacket(buf);
				delay_ms(200);
			}
			buf[0] = 0;
			SI24R1_RX_Mode();
		}

		if(!SI24R1_RxPacket(buf))
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
#endif
}
