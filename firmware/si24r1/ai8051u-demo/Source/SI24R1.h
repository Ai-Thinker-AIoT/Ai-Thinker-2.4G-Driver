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
#define SI24R1_ROLE_TX  1
#define SI24R1_ROLE_RX  0
#ifndef SI24R1_ROLE
#define SI24R1_ROLE     SI24R1_ROLE_TX
#endif

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

#endif
