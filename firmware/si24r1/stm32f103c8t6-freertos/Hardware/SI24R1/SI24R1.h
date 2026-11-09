#ifndef __SI24R1_H
#define __SI24R1_H

#include "stm32f10x.h"

//==============================================================================
// SI24R1 / nRF24L01+ 引脚定义（软件 SPI，按实际接线修改即可）
// 按 LoraKit 转接板实际接线，避开 SWD(PA13/PA14) 与 JTAG 引脚。
//   CE  : PA11  芯片使能（输出，飞线到 CTR1）
//   CSN : PA4   SPI 片选（输出，低有效）
//   SCK : PA5   时钟      （输出）
//   MOSI: PA7   主机发    （输出）
//   MISO: PA6   主机收    （输入）
//   IRQ : PB0   中断输出  （输入，模块开漏，内部上拉）
//==============================================================================
#define SI24R1_CE_GPIO_PORT    GPIOA
#define SI24R1_CE_PIN          GPIO_Pin_11
#define SI24R1_CSN_GPIO_PORT   GPIOA
#define SI24R1_CSN_PIN         GPIO_Pin_4
#define SI24R1_SCK_GPIO_PORT   GPIOA
#define SI24R1_SCK_PIN         GPIO_Pin_5
#define SI24R1_MOSI_GPIO_PORT  GPIOA
#define SI24R1_MOSI_PIN        GPIO_Pin_7
#define SI24R1_MISO_GPIO_PORT  GPIOA
#define SI24R1_MISO_PIN        GPIO_Pin_6
#define SI24R1_IRQ_GPIO_PORT   GPIOB
#define SI24R1_IRQ_PIN         GPIO_Pin_0

// 逻辑电平操作宏（基于 STM32 标准外设库）
#define SI24R1_CE_HIGH()    GPIO_SetBits(SI24R1_CE_GPIO_PORT,  SI24R1_CE_PIN)
#define SI24R1_CE_LOW()     GPIO_ResetBits(SI24R1_CE_GPIO_PORT, SI24R1_CE_PIN)
#define SI24R1_CSN_HIGH()   GPIO_SetBits(SI24R1_CSN_GPIO_PORT, SI24R1_CSN_PIN)
#define SI24R1_CSN_LOW()    GPIO_ResetBits(SI24R1_CSN_GPIO_PORT, SI24R1_CSN_PIN)
#define SI24R1_SCK_HIGH()   GPIO_SetBits(SI24R1_SCK_GPIO_PORT, SI24R1_SCK_PIN)
#define SI24R1_SCK_LOW()    GPIO_ResetBits(SI24R1_SCK_GPIO_PORT, SI24R1_SCK_PIN)
#define SI24R1_MOSI_HIGH()  GPIO_SetBits(SI24R1_MOSI_GPIO_PORT, SI24R1_MOSI_PIN)
#define SI24R1_MOSI_LOW()   GPIO_ResetBits(SI24R1_MOSI_GPIO_PORT, SI24R1_MOSI_PIN)
#define SI24R1_MISO_READ()  GPIO_ReadInputDataBit(SI24R1_MISO_GPIO_PORT, SI24R1_MISO_PIN)
#define SI24R1_IRQ_READ()   GPIO_ReadInputDataBit(SI24R1_IRQ_GPIO_PORT, SI24R1_IRQ_PIN)

//==============================================================================
// 帧格式：地址宽度与有效数据宽度
//==============================================================================
#define TX_ADR_WIDTH    5       // 5 字节地址宽度
#define TX_PLOAD_WIDTH  32      // 32 字节有效数据宽度

//==============================================================================
// SPI(SI24R1) 命令
//==============================================================================
#define SI24R1_READ_REG   0x00    // 读寄存器命令（改名为 SI24R1_READ_REG，避免与 CMSIS stm32f10x.h 的 WRITE_REG/READ_REG 函数宏冲突）
#define SI24R1_WRITE_REG  0x20    // 写寄存器命令
#define RD_RX_PLOAD     0x61    // 读 RX FIFO
#define WR_TX_PLOAD     0xA0    // 写 TX FIFO
#define FLUSH_TX        0xE1    // 清 TX FIFO
#define FLUSH_RX        0xE2    // 清 RX FIFO
#define REUSE_TX_PL     0xE3    // 重发上一包
#define NOP             0xFF    // 空操作（常用于读状态）

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
// STATUS 寄存器中断标志位
//==============================================================================
#define RX_DR           0x40    // 接收到数据
#define TX_DS           0x20    // 数据发送完成
#define MAX_RT          0x10    // 达到最大重发次数

//==============================================================================
// 角色配置：0 = 接收端(RX)，1 = 发送端(TX)
// 两块板必须对设为相反角色才能通信。可在本文件或 Keil 的 C 预定义宏中覆盖。
//==============================================================================
#define SI24R1_ROLE_TX  1
#define SI24R1_ROLE_RX  0
#ifndef SI24R1_ROLE
#define SI24R1_ROLE     SI24R1_ROLE_RX
#endif

//==============================================================================
// 函数原型
//==============================================================================
void SI24R1_Init(void);                         // GPIO 初始化 + DWT 延时使能
u8   SI24R1_Write_Reg(u8 reg, u8 value);
u8   SI24R1_Write_Buf(u8 reg, const u8 *pBuf, u8 bytes);
u8   SI24R1_Read_Reg(u8 reg);
u8   SI24R1_Read_Buf(u8 reg, u8 *pBuf, u8 bytes);

void SI24R1_RX_Mode(void);
void SI24R1_TX_Mode(void);
u8   SI24R1_RxPacket(u8 *rxbuf);
u8   SI24R1_TxPacket(u8 *txbuf);

#endif /* __SI24R1_H */
