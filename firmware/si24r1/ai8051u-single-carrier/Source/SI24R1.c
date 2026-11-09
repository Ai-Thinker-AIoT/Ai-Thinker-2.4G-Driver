#include "SI24R1.h"
#include "INTRINS.H"      // _nop_() 用于软延时；C351 同 Keil C51 内置

#if (SI24R1_TEST_MODE != 0)
//==============================================================================
// 测试模式专用简易毫秒延时：1T/33.1776MHz 粗略值
// 用途：CONFIG 写 PWR_UP 后等待 >=1.5ms 进入 standby（Appendix C 要求）
//==============================================================================
static void SI24R1_DelayMs(u16 ms)
{
    u16 i, j;
    for(i = 0; i < ms; i++)
        for(j = 0; j < 8000; j++)
            _nop_();
}
#endif

//==============================================================================
// 默认通信地址：TX 与 RX 必须使用相同地址，否则无法通信（可按项目修改）
// 注意：C351(32位) 中不能使用 C51 的 code 关键字，改为 const 放入 ROM
//==============================================================================
#if RF_LINK_BUF
const u8 TX_ADDRESS[TX_ADR_WIDTH] = {0x0A, 0x01, 0x07, 0x0E, 0x01};
#endif


//==============================================================================
// 软件 SPI 单字节收发：AI8051U 32位/C351 直接位操作 P1 口，不用硬件 SPI
//   - MOSI 在 SCK 上升沿前送出（SI24R1 在上升沿锁存 MOSI）
//   - MISO 在 SCK 下降沿后读取
//   - 中间插入 _nop_() 因为内核高速(1T/33MHz)，保证建立/保持时间，可按需删减
//==============================================================================
static u8 SPI_RW(u8 byte)
{
    u8 bit_ctr;
    for(bit_ctr = 0; bit_ctr < 8; bit_ctr++)
    {
        if(byte & 0x80)
            MOSI = 1;
        else
            MOSI = 0;
        byte = (byte << 1);
        _nop_(); _nop_();              // MOSI 建立时间
        SCK  = 1;                     // 上升沿：SI24R1 锁存 MOSI
        byte |= MISO;                 // 读取 MISO 到 LSB
        _nop_(); _nop_();             // 保持时间
        SCK  = 0;                     // 下降沿
    }
    return(byte);
}


//==============================================================================
// 函数功能：SI24R1 端口/引脚初始化
// 参    数：无
// 返 回 值：无
// 说    明：AI8051U 与 STC15 不同，端口默认不是准双向，必须显式配置，
//           P10(MISO) 设为输入， P11(IRQ)  设为输入
//           P12(SCK)  设为输出，P13(MOSI) 设为输出
//           P14(CE)   设为输出，P15(CSN)  设为输出
//==============================================================================
void SI24R1_Init(void)
{
    // ---- AI8051U 端口模式配置 ----
    // PxM1=1/PxM0=0 -> 输入(高阻)；  PxM1=0/PxM0=1 -> 推挽输出
    P1M1 = 0x03;   // bit0(P10)=1, bit1(P11)=1  -> MISO/IRQ 输入(高阻)
    P1M0 = 0x0F;   // bit2~5(P12..P15)=1       -> SCK/MOSI/CE/CSN 推挽输出
    P1   = 0xFF;   // 端口输出锁存置1，输入引脚读为1

    // ---- SI24R1 接口初始状态 ----
    SCK = 0;       // SPI 时钟空闲为低
    CSN = 1;       // 取消片选
    CE  = 0;       // 待机模式
    // IRQ 为开漏输出，需外部上拉
}


//============================================================================9
// 函数功能：写 SI24R1 寄存器单字节
// 参    数：reg   -> 寄存器地址（格式 WRITE_REG | reg）
//           value -> 待写值
// 返 回 值：状态寄存器值
//==============================================================================
u8 SI24R1_Write_Reg(u8 reg, u8 value)
{
    u8 status;

    CSN = 0;
    status = SPI_RW(reg);
    SPI_RW(value);
    CSN = 1;

    return(status);
}


//==============================================================================
// 函数功能：写 SI24R1 寄存器多字节（常用于写地址/FIFO）
// 参    数：reg   -> 寄存器地址（格式 WRITE_REG | reg）
//           pBuf  -> 数据源指针
//           bytes -> 字节数
// 返 回 值：状态寄存器值
#if RF_LINK_BUF
//==============================================================================
u8 SI24R1_Write_Buf(u8 reg, const u8 *pBuf, u8 bytes)
{
    u8 status, byte_ctr;

    CSN = 0;
    status = SPI_RW(reg);
    for(byte_ctr = 0; byte_ctr < bytes; byte_ctr++)
        SPI_RW(*pBuf++);
    CSN = 1;

    return(status);
}
#endif


//==============================================================================
// 函数功能：读 SI24R1 寄存器单字节
// 参    数：reg -> 寄存器地址（格式 READ_REG | reg）
// 返 回 值：寄存器值
#if (SI24R1_TEST_MODE == 0)
//==============================================================================
u8 SI24R1_Read_Reg(u8 reg)
{
    u8 value;

    CSN = 0;
    SPI_RW(reg);
    value = SPI_RW(0);
    CSN = 1;

    return(value);
}
#endif


//==============================================================================
// 函数功能：读 SI24R1 寄存器多字节
// 参    数：reg   -> 寄存器地址（格式 READ_REG | reg）
//           pBuf  -> 接收缓冲区指针
//           bytes -> 字节数
// 返 回 值：状态寄存器值
#if RF_LINK_RX
//==============================================================================
u8 SI24R1_Read_Buf(u8 reg, u8 *pBuf, u8 bytes)
{
    u8 status, byte_ctr;

    CSN = 0;
    status = SPI_RW(reg);
    for(byte_ctr = 0; byte_ctr < bytes; byte_ctr++)
        pBuf[byte_ctr] = SPI_RW(0);   // 读取数据，收到数据后地址自增
    CSN = 1;

    return(status);
}
#endif


//==============================================================================
// 函数功能：SI24R1 接收模式初始化
// 参    数：无
// 返 回 值：无
#if RF_LINK_RX
//==============================================================================
void SI24R1_RX_Mode(void)
{
    CE = 0;
    SI24R1_Write_Buf(WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH); // 通道0接收地址，与发送端相同
    SI24R1_Write_Reg(WRITE_REG + EN_AA,     0x01);   // 使能通道0自动应答
    SI24R1_Write_Reg(WRITE_REG + EN_RXADDR, 0x01);   // 使能接收通道0
    SI24R1_Write_Reg(WRITE_REG + RF_CH,    RF_CH_SEL); // 2400 + RF_CH_SEL [MHz]
    SI24R1_Write_Reg(WRITE_REG + RX_PW_P0,  TX_PLOAD_WIDTH); // 通道0有效数据宽度
    SI24R1_Write_Reg(WRITE_REG + RF_SETUP,  RF_SETUP_VALUE); // 速率 + 功率，见 SI24R1.h
    SI24R1_Write_Reg(WRITE_REG + CONFIG,    0x0f);   // CRC使能, 16位CRC, 上电, 接收模式
    SI24R1_Write_Reg(WRITE_REG + STATUS,    0xff);   // 清除所有中断标志
    CE = 1;                                             // 拉高CE进入接收状态
}
#endif


//==============================================================================
// 函数功能：SI24R1 发送模式初始化
// 参    数：无
// 返 回 值：无
#if RF_LINK_TXPKT
//==============================================================================
void SI24R1_TX_Mode(void)
{
    CE = 0;
    SI24R1_Write_Buf(WRITE_REG + TX_ADDR,    TX_ADDRESS, TX_ADR_WIDTH); // 写入发送地址
    SI24R1_Write_Buf(WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH); // 自动应答使用相同地址

    SI24R1_Write_Reg(WRITE_REG + EN_AA,     0x01);   // 使能通道0自动应答
    SI24R1_Write_Reg(WRITE_REG + EN_RXADDR, 0x01);   // 使能接收通道0
    SI24R1_Write_Reg(WRITE_REG + SETUP_RETR,0x1f);   // 自动重发间隔 250us+86us, 最多10次
    SI24R1_Write_Reg(WRITE_REG + RF_CH,    RF_CH_SEL); // 2400 + RF_CH_SEL [MHz]
    SI24R1_Write_Reg(WRITE_REG + RF_SETUP,  RF_SETUP_TX);     // 速率 + 功率（CW 模式含 CONT_WAVE/PLL_LOCK）
    SI24R1_Write_Reg(WRITE_REG + CONFIG,    0x0e);   // CRC使能, 16位CRC, 上电, 发送模式
    // CE 在 TxPacket 中拉高启动发送
}
#endif


//==============================================================================
// 函数功能：接收一帧数据
// 参    数：rxbuf -> 接收缓冲区（>=TX_PLOAD_WIDTH 字节）
// 返 回 值：0 -> 收到数据， 1 -> 未收到
#if RF_LINK_RX
//==============================================================================
u8 SI24R1_RxPacket(u8 *rxbuf)
{
    u8 state;
    state = SI24R1_Read_Reg(STATUS);                 // 读取状态寄存器
    SI24R1_Write_Reg(WRITE_REG + STATUS, state);     // 清除 RX_DR 中断标志

    if(state & RX_DR)                                // 收到数据
    {
        SI24R1_Read_Buf(RD_RX_PLOAD, rxbuf, TX_PLOAD_WIDTH);  // 读取数据
        SI24R1_Write_Reg(FLUSH_RX, 0xff);            // 清空 RX FIFO
        return 0;
    }
    return 1;                                        // 未收到任何数据
}
#endif


//==============================================================================
// 函数功能：发送一帧数据
// 参    数：txbuf -> 待发送数据（TX_PLOAD_WIDTH 字节）
// 返 回 值：0x10(MAX_RT) -> 达到最大重发次数，发送失败
//           0x20(TX_DS)  -> 发送成功
//           0xFF         -> 发送失败，其他原因
#if RF_LINK_TXPKT
//==============================================================================
u8 SI24R1_TxPacket(u8 *txbuf)
{
    u8 state; u16 t;
    CE = 0;                                                          // CE 拉低，进入待机
    SI24R1_Write_Buf(WR_TX_PLOAD, txbuf, TX_PLOAD_WIDTH);             // 写入 TX FIFO(32字节)
    CE = 1;                                                          // CE 拉高，启动发送

    t = 0; while((IRQ == 1) && (++t));   // wait IRQ low, with timeout (no module = no hang)
    state = SI24R1_Read_Reg(STATUS);                                 // 读取状态寄存器
    SI24R1_Write_Reg(WRITE_REG + STATUS, state);                     // 清除 TX_DS / MAX_RT 中断标志

    if(state & MAX_RT)                                               // 达到最大重发次数
    {
        SI24R1_Write_Reg(FLUSH_TX, 0xff);                            // 清空 TX FIFO
        return MAX_RT;
    }
    if(state & TX_DS)                                                // 发送成功
    {
        return TX_DS;
    }
    return 0xFF;                                                     // 发送失败
}
#endif


#if (SI24R1_TEST_MODE == 1)
//==============================================================================
// 恒载波（未调制单载波）模式，严格按 nRF24L01+ 规格书 Appendix C：
//   CONFIG: PWR_UP=1, PRIM_RX=0 -> 等待 >=1.5ms 进入 standby
//   RF_SETUP: CONT_WAVE=1 + PLL_LOCK=1；设置 RF_CH；CE 保持高电平
// 用于载波功率 / 频率容限 / 杂散测试。进入后载波持续输出，不返回主循环。
//==============================================================================
void SI24R1_CW_Mode(void)
{
    CE = 0;
    SI24R1_Write_Reg(WRITE_REG + CONFIG,   0x0e);
    SI24R1_DelayMs(2);
    SI24R1_Write_Reg(WRITE_REG + RF_SETUP, RF_SETUP_TX);
    SI24R1_Write_Reg(WRITE_REG + RF_CH,    RF_CH_SEL);
    CE = 1;
}
#endif

#if (SI24R1_TEST_MODE == 2)
//==============================================================================
// 连续调制载波，用于占用带宽 / 6dB 带宽 / PSD 测试
//   逐包调用 SI24R1_TxPacket() 会形成通断突发，扫频仪测不准。这里按 Appendix C：
//     - EN_AA / SETUP_RETR 清零，不等 ACK、不自动重发
//     - CE 保持高电平，发射机不回 standby
//     - 持续向 TX FIFO 喂伪随机载荷（不能全 0x00/0xFF，否则谱线塌缩）
//   txbuf: 32 字节载荷缓冲（内容被本函数覆盖）；count: 包数，0 = 无限
//==============================================================================
void SI24R1_Continuous_Tx_Test(u8 *txbuf, u16 count)
{
    u16 i, j;
    u8  seed = 0x5a;

    CE = 0;
    SI24R1_Write_Reg(WRITE_REG + CONFIG,    0x0e);   // PWR_UP=1, PRIM_RX=0, 16位 CRC
    SI24R1_DelayMs(2);                              // PWR_UP -> standby >=1.5ms

    SI24R1_Write_Buf(WRITE_REG + TX_ADDR,   TX_ADDRESS, TX_ADR_WIDTH);
    SI24R1_Write_Reg(WRITE_REG + EN_AA,     0x00);   // 关自动应答
    SI24R1_Write_Reg(WRITE_REG + SETUP_RETR,0x00);   // 关自动重发
    SI24R1_Write_Reg(WRITE_REG + RF_SETUP,  RF_SETUP_TX);
    SI24R1_Write_Reg(WRITE_REG + RF_CH,     RF_CH_SEL);
    SI24R1_Write_Reg(WRITE_REG + STATUS,    0xff);   // 清中断标志

    CE = 1;                                          // 发射机启动，之后不再翻转
    for(i = 0; ; i++)
    {
        for(j = 0; j < TX_PLOAD_WIDTH; j++)
        {
            seed = (u8)(seed * 0x1d + 0x37);
            txbuf[j] = seed;
        }
        SI24R1_Write_Buf(WR_TX_PLOAD, txbuf, TX_PLOAD_WIDTH);
        if(count && (i + 1) >= count) break;
    }
    CE = 0;
}
#endif
