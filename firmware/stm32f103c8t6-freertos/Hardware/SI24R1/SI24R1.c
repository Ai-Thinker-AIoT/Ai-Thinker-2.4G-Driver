#include "SI24R1.h"

/* DWT 周期计数器延时支持 ---------------------------------------------------
 * 本工程 Start\core_cm3.h 未定义 DWT / DWT_Type / DWT_CTRL_CYCCNTENA_Msk（较旧 CMSIS），
 * 而本文件的微秒延时依赖 DWT。此处按 ARM 内存映射补齐，并用 #ifndef 守卫避免冲突。 */
#ifndef DWT_BASE
#define DWT_BASE                0xE0001000UL
#define DWT_CTRL_CYCCNTENA_Msk  (1UL << 0)
typedef struct {
    volatile uint32_t CTRL;     /* 0x00 Control Register */
    volatile uint32_t CYCCNT;   /* 0x04 Cycle Count Register */
} DWT_Type;
#define DWT                     ((DWT_Type *)DWT_BASE)
#endif

// 默认通信地址（与对端保持一致即可，这里沿用原 STC 工程的地址）
const u8 TX_ADDRESS[TX_ADR_WIDTH] = {0x0A, 0x01, 0x07, 0x0E, 0x01};

//==============================================================================
// 微秒级延时（基于 DWT 周期计数器，不占用 SysTick，可在 FreeRTOS 任务中安全使用）
//==============================================================================
static void SI24R1_DWT_Init(void)
{
    if (!(CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk))
    {
        CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    }
    DWT->CYCCNT  = 0;
    DWT->CTRL   |= DWT_CTRL_CYCCNTENA_Msk;
}

static void SI24R1_DelayUs(uint32_t us)
{
    uint32_t start  = DWT->CYCCNT;
    uint32_t cycles = us * (SystemCoreClock / 1000000);
    while ((DWT->CYCCNT - start) < cycles);
}

//==============================================================================
// 软件 SPI：单字节收发（模式 0，MSB 先行）
// 内部已加 1us 半周期延时，使 SCK 约 500kHz~1MHz，远低于 SI24R1 上限 10MHz。
//==============================================================================
static u8 SPI_RW(u8 byte)
{
    u8 bit_ctr;

    for (bit_ctr = 0; bit_ctr < 8; bit_ctr++)
    {
        if (byte & 0x80)
            SI24R1_MOSI_HIGH();
        else
            SI24R1_MOSI_LOW();

        byte = (byte << 1);

        SI24R1_SCK_HIGH();
        SI24R1_DelayUs(1);

        if (SI24R1_MISO_READ())
            byte |= 0x01;

        SI24R1_SCK_LOW();
        SI24R1_DelayUs(1);
    }

    return byte;
}

//==============================================================================
// 引脚初始化
//==============================================================================
void SI24R1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);

    // 输出引脚：CE / CSN / SCK / MOSI（逐脚初始化，端口可能不同）
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin = SI24R1_CE_PIN;
    GPIO_Init(SI24R1_CE_GPIO_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = SI24R1_CSN_PIN;
    GPIO_Init(SI24R1_CSN_GPIO_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = SI24R1_SCK_PIN;
    GPIO_Init(SI24R1_SCK_GPIO_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = SI24R1_MOSI_PIN;
    GPIO_Init(SI24R1_MOSI_GPIO_PORT, &GPIO_InitStructure);

    // 输入引脚：MISO（浮空即可，模块无内部上拉）
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin  = SI24R1_MISO_PIN;
    GPIO_Init(SI24R1_MISO_GPIO_PORT, &GPIO_InitStructure);

    // 输入引脚：IRQ（模块内部开漏；配置为带弱上拉输入）
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin  = SI24R1_IRQ_PIN;
    GPIO_Init(SI24R1_IRQ_GPIO_PORT, &GPIO_InitStructure);

    // 初始电平：SPI 空闲（CSN 高、SCK 低、CE 低）
    SI24R1_SCK_LOW();
    SI24R1_CSN_HIGH();
    SI24R1_CE_LOW();

    SI24R1_DWT_Init();   // 使能 DWT 周期计数器供延时使用
}

//==============================================================================
// 写单个寄存器
//==============================================================================
u8 SI24R1_Write_Reg(u8 reg, u8 value)
{
    u8 status;

    SI24R1_CSN_LOW();
    status = SPI_RW(reg);
    SPI_RW(value);
    SI24R1_CSN_HIGH();

    return status;
}

//==============================================================================
// 写多个字节（寄存器 + 数据）
//==============================================================================
u8 SI24R1_Write_Buf(u8 reg, const u8 *pBuf, u8 bytes)
{
    u8 status, byte_ctr;

    SI24R1_CSN_LOW();
    status = SPI_RW(reg);
    for (byte_ctr = 0; byte_ctr < bytes; byte_ctr++)
        SPI_RW(*pBuf++);
    SI24R1_CSN_HIGH();

    return status;
}

//==============================================================================
// 读单个寄存器
//==============================================================================
u8 SI24R1_Read_Reg(u8 reg)
{
    u8 value;

    SI24R1_CSN_LOW();
    SPI_RW(reg);
    value = SPI_RW(0);
    SI24R1_CSN_HIGH();

    return value;
}

//==============================================================================
// 读多个字节
//==============================================================================
u8 SI24R1_Read_Buf(u8 reg, u8 *pBuf, u8 bytes)
{
    u8 status, byte_ctr;

    SI24R1_CSN_LOW();
    status = SPI_RW(reg);
    for (byte_ctr = 0; byte_ctr < bytes; byte_ctr++)
        pBuf[byte_ctr] = SPI_RW(0);
    SI24R1_CSN_HIGH();

    return status;
}

//==============================================================================
// 接收模式配置
//==============================================================================
void SI24R1_RX_Mode(void)
{
    SI24R1_CE_LOW();
    SI24R1_Write_Buf(SI24R1_WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH); // 通道0地址
    SI24R1_Write_Reg(SI24R1_WRITE_REG + EN_AA,       0x01);                  // 通道0自动应答
    SI24R1_Write_Reg(SI24R1_WRITE_REG + EN_RXADDR,   0x01);                  // 使能通道0
    SI24R1_Write_Reg(SI24R1_WRITE_REG + RF_CH,      100);                    // 射频通道 40
    SI24R1_Write_Reg(SI24R1_WRITE_REG + RX_PW_P0,    TX_PLOAD_WIDTH);        // 通道0数据宽度
    SI24R1_Write_Reg(SI24R1_WRITE_REG + RF_SETUP,    0x07);                  // 1Mbps, 0dBm, LNA high
    SI24R1_Write_Reg(SI24R1_WRITE_REG + CONFIG,      0x0F);                  // CRC16, 上电, 接收模式
    SI24R1_Write_Reg(SI24R1_WRITE_REG + STATUS,      0xFF);                  // 清所有中断标志
    SI24R1_CE_HIGH();                                                 // CE 拉高进入接收
}

//==============================================================================
// 发送模式配置
//==============================================================================
void SI24R1_TX_Mode(void)
{
    SI24R1_CE_LOW();
    SI24R1_Write_Buf(SI24R1_WRITE_REG + TX_ADDR,    TX_ADDRESS, TX_ADR_WIDTH); // 发送地址
    SI24R1_Write_Buf(SI24R1_WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH); // 自动应答用同地址
    SI24R1_Write_Reg(SI24R1_WRITE_REG + EN_AA,      0x01);                   // 通道0自动应答
    SI24R1_Write_Reg(SI24R1_WRITE_REG + EN_RXADDR,  0x01);                   // 使能通道0
    SI24R1_Write_Reg(SI24R1_WRITE_REG + SETUP_RETR, 0x1F);                   // 重发: 500us, 10次
    SI24R1_Write_Reg(SI24R1_WRITE_REG + RF_CH,     100);                     // 射频通道 40
    SI24R1_Write_Reg(SI24R1_WRITE_REG + RF_SETUP,   0x07);                   // 1Mbps, 0dBm, LNA high
    SI24R1_Write_Reg(SI24R1_WRITE_REG + CONFIG,     0x0E);                   // CRC16, 上电, 发送模式
    // CE 在发送时由 TxPacket 控制脉冲
}

//==============================================================================
// 接收一包数据
// 返回 0: 收到数据;  1: 无数据
//==============================================================================
u8 SI24R1_RxPacket(u8 *rxbuf)
{
    u8 state;

    state = SI24R1_Read_Reg(STATUS);            // 读状态
    SI24R1_Write_Reg(SI24R1_WRITE_REG + STATUS, state); // 清 RX_DR 标志

    if (state & RX_DR)
    {
        SI24R1_Read_Buf(RD_RX_PLOAD, rxbuf, TX_PLOAD_WIDTH);
        SI24R1_Write_Reg(FLUSH_RX, 0xFF);       // 清 RX FIFO
        return 0;
    }
    return 1;
}

//==============================================================================
// 发送一包数据
// 返回 MAX_RT(0x10): 重发失败;  TX_DS(0x20): 发送成功;  0xFF: 其他原因
//==============================================================================
u8 SI24R1_TxPacket(u8 *txbuf)
{
    u8 state;

    SI24R1_CE_LOW();
    SI24R1_Write_Buf(WR_TX_PLOAD, txbuf, TX_PLOAD_WIDTH); // 写入 TX FIFO
    SI24R1_CE_HIGH();                                    // CE 拉高启动发送
    // 等待 IRQ 拉低（TX_DS / MAX_RT 都会触发），50ms 超时保护（模块未接/无应答时不死等）
    {
        uint32_t t_start = DWT->CYCCNT;
        while (SI24R1_IRQ_READ() == 1)
        {
            if ((DWT->CYCCNT - t_start) > 50UL * (SystemCoreClock / 1000UL))
                break;
        }
    }
    SI24R1_CE_LOW();

    state = SI24R1_Read_Reg(STATUS);             // 读状态
    SI24R1_Write_Reg(SI24R1_WRITE_REG + STATUS, state); // 清 TX_DS / MAX_RT 标志

    if (state & MAX_RT)
    {
        SI24R1_Write_Reg(FLUSH_TX, 0xFF);        // 清 TX FIFO
        return MAX_RT;
    }
    if (state & TX_DS)
    {
        return TX_DS;
    }
    return 0xFF;
}
