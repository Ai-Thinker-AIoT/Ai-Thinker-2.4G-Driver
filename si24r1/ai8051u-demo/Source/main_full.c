/* ============================================================================
 * main.c  AI8051U (32位 / 长缨-I C351 编译器) + SI24R1 无线收发演示
 * ---------------------------------------------------------------------------
 * 编译工具：长缨-I(C351) 32位编译器  ->  直接读取项目目录 AI8051U-SI24R1.uvproj
 * 芯片配置：STC51U_8BIT (AI8051U 32位模式)
 * 默认时钟：33.1776MHz (1T)；UART1 = 115200-8-N-1
 * 信号分配：SI24R1 挂在 P1 口（见 SI24R1.h 定义）
 *
 * 使用说明：
 *   1. 两块板要通信，必须使用同一份地址，且角色必须相反才能互通：
 *        - 发送端：SI24R1.h 中 SI24R1_ROLE 设为 SI24R1_ROLE_TX (1)
 *        - 接收端：SI24R1.h 中 SI24R1_ROLE 设为 SI24R1_ROLE_RX (0)
 *      也可在编译器预定义里加 -DSI24R1_ROLE=0 / 1 覆盖。
 *   2. 演示用状态 LED 默认 P40（低电平点亮），可改接其它 LED 脚。
 * ========================================================================== */

// ---- C351 芯片定义（关键：选定 AI8051U 32位模式）----
#define AI8051U34K64   MCU_DEFINE  STC51U_8BIT   32760

// ---- 系统参数 ----
#define MAIN_Fosc      33177600L     // 主频 33.1776MHz
#define UART1_Baud     115200L       // 串口波特率

// ---- 头文件 ----
#include "XSFR_STC8051U_8Bit.H"     // AI8051U 寄存器/端口定义（同目录）
#include "SI24R1.h"                 // SI24R1 驱动
#include "C351_STDIO.H"             // puts / putchar
#include "C351_PRTSCN.H"            // printf / sprintf
#include "INTRINS.H"                // _nop_()

// ---- 演示用状态 LED（低电平点亮；可按实际板载 LED 脚修改）----
// 注意：C351 中 sbit 直接取寄存器位；本演示 LED 实际在 P20 或 P40 由硬件决定，
//       若 LED 实际在 P20 则改 P2^0；在 P40 则改 P4^0。
sbit LED = P2^0;


//==============================================================================
// 简单软件延时：1T/33.1776MHz 下的粗略值，非精确计时
//==============================================================================
void DelayMs(u16 ms)
{
    u16 i, j;
    for(i = 0; i < ms; i++)
        for(j = 0; j < 8000; j++)
            _nop_();
}


//==============================================================================
// UART1 初始化：115200 @ 33.1776MHz，Timer2(1T) 作波特率发生器
// 计算公式：重载值 = 65536 - Fosc / (4 * 波特率)
//          = 65536 - 33177600 / (4 * 115200) = 65536 - 72 = 65464 = 0xFFB8
//==============================================================================
void Uart1_Init(void)
{
    P_SW1 |= 0x40;       // UART1 重映射：P36(TXD)/P37(RXD)；00=默认P30/P31, 01=P36/P37
    SCON  = 0x50;        // 8位数据，可变波特率
    AUXR |= 0x01;        // UART1 选择 Timer2 为波特率发生器
    AUXR |= 0x04;        // Timer2 1T 模式
    TL2   = 0xB8;        // 定时器初值低字节
    TH2   = 0xFF;        // 定时器初值高字节
    AUXR |= 0x10;        // 启动 Timer2
}


//==============================================================================
// C351 printf 底层：定向到 UART1
//==============================================================================
void putchar(char x)
{
    SBUF = x;
    while(TI == 0);
    TI = 0;
}


//==============================================================================
// 主函数
//==============================================================================
void main(void)
{
    u8 txbuf[TX_PLOAD_WIDTH];
    u8 rxbuf[TX_PLOAD_WIDTH];
    u8 sta, i, cnt = 0;

    EA = 0;                          // 关闭中断
    P2M1 = 0x00; P2M0 = 0x00;        // P2 must be set explicitly (AI8051U not quasi by default)
    P4M1 = 0x00; P4M0 = 0x00;        // P4 准双向（LED 用）
    LED = 1;                         // LED 灭（默认低电平点亮）

    Uart1_Init();
    TI = 1; ES = 0;                  // 允许 printf 并关闭串口中断

    // ===== 启动自检（诊断用，临时）=====
    // 先闪 8 次 P2.0(LED) 与 P4.0，确认 MCU 已跑进 main
    // 这一步在正式发送/接收逻辑之前；确认固件能跑通后，可删掉本段。
    {
        volatile unsigned long k;
        unsigned char n;
        for (n = 0; n < 8; n++)
        {
            LED = 0; P40 = 0; for (k = 0; k < 1500000UL; k++);
            LED = 1; P40 = 1; for (k = 0; k < 1500000UL; k++);
        }
    }

    printf("\r\n================================\r\n");
    printf(" AI8051U(32bit) + SI24R1 Demo\r\n");
    printf(" Role = %s\r\n", SI24R1_ROLE ? "TX" : "RX");
    printf("================================\r\n");

#if (SI24R1_ROLE == SI24R1_ROLE_TX)
    // ---------------------- 发送端 ----------------------
    SI24R1_TX_Mode();
    printf(">> TX mode ready, sending...\r\n");
    while(1)
    {
        // 构造一帧测试数据
        for(i = 0; i < TX_PLOAD_WIDTH; i++)
            txbuf[i] = cnt + i;

        sta = SI24R1_TxPacket(txbuf);
        if(sta == TX_DS)
        {
            LED = 0; DelayMs(60); LED = 1;     // 发送成功闪一下
            printf("TX ok [%03u] %02X %02X %02X ...\r\n",
                   (u16)cnt, txbuf[0], txbuf[1], txbuf[2]);
        }
        else if(sta == MAX_RT)
            printf("TX max_retry (no ACK)\r\n");
        else
            printf("TX fail (0x%02X)\r\n", sta);

        cnt++;
        DelayMs(500);
    }

#else
    // ---------------------- 接收端 ----------------------
    SI24R1_RX_Mode();
    printf(">> RX mode ready, waiting...\r\n");
    while(1)
    {
        sta = SI24R1_RxPacket(rxbuf);
        if(sta == 0)
        {
            LED = 0; DelayMs(60); LED = 1;     // 收到数据闪一下
            printf("RX: ");
            for(i = 0; i < TX_PLOAD_WIDTH; i++)
                printf("%02X ", rxbuf[i]);
            printf("\r\n");
        }
        DelayMs(10);
    }
#endif
}
