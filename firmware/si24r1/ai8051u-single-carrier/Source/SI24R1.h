#ifndef __SI24R1_H
#define __SI24R1_H

#include "XSFR_STC8051U_8Bit.H"   // AI8051U 寄存器/端口定义（长缨 C351 32位编译器）

//==============================================================================
// SI24R1 引脚定义（软件 SPI，挂在 AI8051U 的 P1 口，可按接线修改）
//   MOSI = P1.3   主机发（输出）
//   MISO = P1.0   主机收（输入）
//   SCK  = P1.2   时钟  （输出）
//   CSN  = P1.5   片选  （输出，低有效）
//   CE   = P1.4   芯片使能（输出）
//   IRQ  = P1.1   中断  （输入，模块开漏，需上拉）
// 头文件已定义 P10..P17 等 sbit，这里用宏起别名，保持驱动代码与 STC15 版一致。
//==============================================================================
#define MOSI  P13
#define MISO  P10
#define SCK   P12
#define CSN   P15
#define CE    P14
#define IRQ   P11

//==============================================================================
// 帧格式
//==============================================================================
#define TX_ADR_WIDTH   5     // 5 字节地址宽度
#define TX_PLOAD_WIDTH 32    // 32 字节有效数据宽度

//==============================================================================
// SPI(SI24R1) 命令
//==============================================================================
#define READ_REG        0x00
#define WRITE_REG       0x20
#define RD_RX_PLOAD     0x61
#define WR_TX_PLOAD     0xA0
#define FLUSH_TX        0xE1
#define FLUSH_RX        0xE2
#define REUSE_TX_PL     0xE3
#define SI24R1_NOP      0xFF   // SI24R1 空操作命令（避免与 XSFR 中函数式宏 NOP(N) 重名冲突 C317）

//==============================================================================
// SPI(SI24R1) 寄存器地址
//==============================================================================
#define CONFIG          0x00
#define EN_AA           0x01
#define EN_RXADDR       0x02
#define SETUP_AW        0x03
#define SETUP_RETR      0x04
#define RF_CH           0x05
#define RF_SETUP        0x06
#define STATUS          0x07
#define OBSERVE_TX      0x08
#define RSSI            0x09
#define RX_ADDR_P0      0x0A
#define RX_ADDR_P1      0x0B
#define RX_ADDR_P2      0x0C
#define RX_ADDR_P3      0x0D
#define RX_ADDR_P4      0x0E
#define RX_ADDR_P5      0x0F
#define TX_ADDR         0x10
#define RX_PW_P0        0x11
#define RX_PW_P1        0x12
#define RX_PW_P2        0x13
#define RX_PW_P3        0x14
#define RX_PW_P4        0x15
#define RX_PW_P5        0x16
#define FIFO_STATUS     0x17

//==============================================================================
// STATUS 寄存器中断标志
//==============================================================================
#define RX_DR   0x40
#define TX_DS   0x20
#define MAX_RT  0x10

//==============================================================================
// 角色配置：0 = 接收端(RX)，1 = 发送端(TX)
// 两块板必须对设为相反角色才能通信。可在本文件或编译器预定义中覆盖。
//==============================================================================
#define SI24R1_ROLE_RX  0
#define SI24R1_ROLE_TX  1
#ifndef SI24R1_ROLE
#define SI24R1_ROLE     SI24R1_ROLE_TX
#endif

//==============================================================================
// RF_SETUP (0x06) 位定义（Si24R1 / nRF24L01+ 兼容）
//   bit7 CONT_WAVE   1 = 未调制单载波，仅测试用
//   bit6 保留，必须为 0
//   bit5 RF_DR_LOW   [bit5,bit3] 00=1Mbps  01=2Mbps  10=250kbps
//   bit4 PLL_LOCK    1 = 强制 PLL 锁定，CW 输出必须置 1
//   bit3 RF_DR_HIGH  Si24R1 数据手册把 10 档标为 205kbps，Nordic 标 250kbps
//   bit2:0 RF_PWR    Si24R1 为 3bit/8 档（nRF24L01+ 只有 2bit/4 档，勿混用）
//==============================================================================
#define RF_RATE_1M      0x00    // 占用带宽约 1 MHz
#define RF_RATE_2M      0x08    // 占用带宽约 2 MHz
#define RF_RATE_250K    0x20    // 占用带宽约 0.43 MHz，实测低于 500kHz 判据
#define RF_RATE_205K    RF_RATE_250K
#define RF_CONT_WAVE    0x80
#define RF_PLL_LOCK     0x10

// Si24R1 功率表：RF_PWR[2:0] 111 110 101 100 011 010 001 000
//                        +7  +4  +3  +1   0  -4  -6 -12 dBm
#define RF_PWR_7DBM     0x07
#define RF_PWR_4DBM     0x06
#define RF_PWR_3DBM     0x05
#define RF_PWR_1DBM     0x04
#define RF_PWR_0DBM     0x03
#define RF_PWR_N4DBM    0x02
#define RF_PWR_N6DBM    0x01
#define RF_PWR_N12DBM   0x00
#define RF_PWR_SEL      RF_PWR_7DBM   // +7 dBm

//==============================================================================
// 定频 / 认证测试模式（同步自 si24r1-DZBMS 工程）
//   0 = 正常收发链路（默认，保持原有 TX/RX 演示不变）
//   1 = CW  未调制单载波：测载波功率 / 频率容限 / 杂散。
//           单根谱线，OBW 仅几十 kHz，永远不可能 >500 kHz。
//   2 = MOD 连续调制发射：测占用带宽 / 6dB 带宽 / PSD。
//           CE 常高 + 持续喂 TX FIFO 才是真正连续的调制载波；
//           逐包发送是通断突发，扫频式频谱仪测不准。
//==============================================================================
#define SI24R1_TEST_MODE    0

// 信道：F0 = 2400 + RF_CH [MHz]
#define RF_CH_2400      0
#define RF_CH_2405      5
#define RF_CH_2440      40
#define RF_CH_2475      75
#define RF_CH_2500      100

// 正常链路沿用原有配置：1Mbps / +7dBm / ch100(2500MHz)；
// 测试模式沿用 DZBMS 认证配置：2Mbps / +7dBm / ch75(2475MHz)。
#if (SI24R1_TEST_MODE == 0)
#define RF_RATE_SEL     RF_RATE_1M
#define RF_CH_SEL       RF_CH_2500
#else
#define RF_RATE_SEL     RF_RATE_2M
#define RF_CH_SEL       RF_CH_2475
#endif

#define RF_SETUP_VALUE  (RF_RATE_SEL | RF_PWR_SEL)
#if (SI24R1_TEST_MODE == 1)
#define RF_SETUP_TX     (RF_SETUP_VALUE | RF_CONT_WAVE | RF_PLL_LOCK)  // CW + PLL_LOCK
#else
#define RF_SETUP_TX     (RF_SETUP_VALUE)
#endif

//---- 按测试模式 / 角色裁剪未调用的函数，消除链接器 L16 警告 ----
//   mode 0 + TX 角色 : TX 包路径；mode 0 + RX 角色 : RX 包路径
//   mode 1 (CW)      : 只留底层寄存器写
//   mode 2 (MOD)     : 只留底层寄存器/缓冲区写
#if (SI24R1_TEST_MODE == 0)
#if (SI24R1_ROLE == SI24R1_ROLE_TX)
#define RF_LINK_RX      0
#define RF_LINK_TXPKT   1
#else
#define RF_LINK_RX      1
#define RF_LINK_TXPKT   0
#endif
#else
#define RF_LINK_RX      0
#define RF_LINK_TXPKT   0
#endif
#define RF_LINK_BUF     (SI24R1_TEST_MODE != 1)   // Write_Buf / TX_ADDRESS 在 CW 模式不用

//==============================================================================
// 函数原型
//==============================================================================
void SI24R1_Init(void);
u8   SI24R1_Write_Reg(u8 reg, u8 value);
u8   SI24R1_Write_Buf(u8 reg, const u8 *pBuf, u8 bytes);
u8   SI24R1_Read_Reg(u8 reg);
u8   SI24R1_Read_Buf(u8 reg, u8 *pBuf, u8 bytes);

void SI24R1_RX_Mode(void);
void SI24R1_TX_Mode(void);
u8   SI24R1_RxPacket(u8 *rxbuf);
u8   SI24R1_TxPacket(u8 *txbuf);
void SI24R1_CW_Mode(void);                              // 未调制单载波（mode 1）
void SI24R1_Continuous_Tx_Test(u8 *txbuf, u16 count);  // 连续调制发射（mode 2, count=0 无限）

#endif
