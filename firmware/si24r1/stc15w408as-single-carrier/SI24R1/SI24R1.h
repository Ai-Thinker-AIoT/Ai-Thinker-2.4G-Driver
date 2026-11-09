#ifndef __nRF24L01P__
#define __nRF24L01P__

#include "config.h"

//SI24R1 PIN DEFINITION
#define	MOSI      P13             // Master Out, Slave In pin (output)
#define	MISO      P10             // Master In, Slave Out pin (input)
#define	SCK       P12             // Serial Clock pin, (output)
#define	CSN       P15             // Slave Select pin, (output to CSN)
#define	CE        P14             // Chip Enable pin signal (output)
#define	IRQ       P11             // Interrupt signal, from nRF24L01 (input)

#define TX_ADR_WIDTH   5  				// 5字节宽度的发送/接收地址
#define TX_PLOAD_WIDTH 32  				// 数据通道有效数据宽度

//********************************************************************************************************************//
// SPI(SI24R1) commands
#define READ_REG        0x00  // Define read command to register
#define WRITE_REG       0x20  // Define write command to register
#define RD_RX_PLOAD     0x61  // Define RX payload register address
#define WR_TX_PLOAD     0xA0  // Define TX payload register address
#define FLUSH_TX        0xE1  // Define flush TX register command
#define FLUSH_RX        0xE2  // Define flush RX register command
#define REUSE_TX_PL     0xE3  // Define reuse TX payload register command
#define NOP             0xFF  // Define No Operation, might be used to read status register

//********************************************************************************************************************//
// SPI(SI24R1) registers(addresses)
#define CONFIG          0x00  // 'Config' register address
#define EN_AA           0x01  // 'Enable Auto Acknowledgment' register address
#define EN_RXADDR       0x02  // 'Enabled RX addresses' register address
#define SETUP_AW        0x03  // 'Setup address width' register address
#define SETUP_RETR      0x04  // 'Setup Auto. Retrans' register address
#define RF_CH           0x05  // 'RF channel' register address
#define RF_SETUP        0x06  // 'RF setup' register address
#define STATUS          0x07  // 'Status' register address
#define OBSERVE_TX      0x08  // 'Observe TX' register address
#define RSSI            0x09  // 'Received Signal Strength Indecator' register address
#define RX_ADDR_P0      0x0A  // 'RX address pipe0' register address
#define RX_ADDR_P1      0x0B  // 'RX address pipe1' register address
#define RX_ADDR_P2      0x0C  // 'RX address pipe2' register address
#define RX_ADDR_P3      0x0D  // 'RX address pipe3' register address
#define RX_ADDR_P4      0x0E  // 'RX address pipe4' register address
#define RX_ADDR_P5      0x0F  // 'RX address pipe5' register address
#define TX_ADDR         0x10  // 'TX address' register address
#define RX_PW_P0        0x11  // 'RX payload width, pipe0' register address
#define RX_PW_P1        0x12  // 'RX payload width, pipe1' register address
#define RX_PW_P2        0x13  // 'RX payload width, pipe2' register address
#define RX_PW_P3        0x14  // 'RX payload width, pipe3' register address
#define RX_PW_P4        0x15  // 'RX payload width, pipe4' register address
#define RX_PW_P5        0x16  // 'RX payload width, pipe5' register address
#define FIFO_STATUS     0x17  // 'FIFO Status Register' register address

//********************************************************************************************************************//
// STATUS Register 
#define RX_DR						0x40  /**/
#define TX_DS						0x20
#define MAX_RT					0x10

//********************************************************************************************************************//
// RF_SETUP (0x06) field encoding -- Si24R1 / nRF24L01+ compatible
//   bit7     CONT_WAVE   1 = unmodulated carrier, test only
//   bit6     reserved    must be 0
//   bit5     RF_DR_LOW   data rate select, see bit3
//   bit4     PLL_LOCK    force PLL lock, REQUIRED for CW output
//   bit3     RF_DR_HIGH  [bit5,bit3] '00'=1Mbps '01'=2Mbps '10'=250kbps
//   bit2:0   RF_PWR      Si24R1 8 levels, see RF_PWR_SEL below
//********************************************************************************************************************//
#define RF_RATE_1M      0x00    // occupied BW ~1 MHz
#define RF_RATE_2M      0x08    // occupied BW ~2 MHz
#define RF_RATE_250K    0x20    // lowest rate, RF_DR encoding [bit5,bit3] = '10'

// Same register setting, two different names in the two datasheets:
//   Si24R1 (Nanjing Zhongke Micro) datasheet calls this mode 205 kbps
//   Nordic nRF24L01+ datasheet calls the identical RF_DR encoding '10' 250 kbps
// Only the label differs -- the byte written to RF_SETUP is 0x20 either way.
#define RF_RATE_205K    RF_RATE_250K
#define RF_CONT_WAVE    0x80    // '1' = unmodulated carrier, test only
#define RF_PLL_LOCK     0x10    // '1' = force PLL lock, REQUIRED for CW output

// Si24R1 output power table (RF_PWR is 3 bits on Si24R1, 2 bits on nRF24L01+)
//   RF_PWR[2:0]   111     110     101     100     011     010     001     000
//   Si24R1      +7dBm   +4dBm   +3dBm   +1dBm    0dBm  -4dBm   -6dBm  -12dBm
//   (Nordic nRF24L01+ only implements 111=0dBm, 110=-6dBm, 101=-12dBm, 100=-18dBm,
//    so the same byte means a very different power on the two parts -- do not mix them.)
#define RF_PWR_7DBM     0x07
#define RF_PWR_4DBM     0x06
#define RF_PWR_3DBM     0x05
#define RF_PWR_1DBM     0x04
#define RF_PWR_0DBM     0x03
#define RF_PWR_N4DBM    0x02
#define RF_PWR_N6DBM    0x01
#define RF_PWR_N12DBM   0x00

#define RF_PWR_SEL      RF_PWR_7DBM   // +7 dBm

// ---- on-air data rate -> occupied bandwidth ----
// Criterion CONFIRMED: 'occupied bandwidth' = the band containing 99% of the
// total mean power. NOT the x dB bandwidth, NOT the 6 dB bandwidth.
// Requirement: occupied bandwidth must be GREATER than 500 kHz.
//   rate        99% OBW        verdict
//   205 kbps    ~0.43 MHz      FAIL  (measured 433.44 kHz on this board)
//   1 Mbps      ~1.0 MHz       OK    <-- selected, roughly 2x margin
//   2 Mbps      ~2.0 MHz       OK,    widest margin but worse sensitivity
#define RF_RATE_SEL     RF_RATE_2M

#define RF_SETUP_VALUE  (RF_RATE_SEL | RF_PWR_SEL)    // 0x0F: 2 Mbps, +7 dBm

// ---- RF channel - selection - 0..125, F0 = 2400 + RF_CH [MHz] ----
// The channel frequency has a 1 MHz programming grid. At 2 Mbps the
// occupied bandwidth is ~2 MHz, wider than that grid, so adjacent 1 MHz
// channels overlap. Irrelevant for a conducted certification test.
#define RF_CH_2400      0
#define RF_CH_2402      2
#define RF_CH_2405      5
#define RF_CH_2440      40
#define RF_CH_2475      75      // <-- selected
#define RF_CH_SEL       RF_CH_2475

// ---- fixed-frequency / certification test mode ----
//   0 = normal packet link (original key-press demo)
//   1 = CW  : unmodulated carrier. For carrier power / frequency tolerance / spurious.
//             A CW is a single spectral line -> occupied BW is ~30 kHz,
//             it can NEVER be greater than 500 kHz.
//   2 = MOD : continuous modulated TX. For occupied BW / 6 dB BW / PSD.
//             This is the ONLY mode that can produce BW > 500 kHz.
//             CE is held high and the TX FIFO is kept fed, so the carrier
//             is modulated without interruption. A burst train cannot be
//             measured correctly by a swept analyser.
#define SI24R1_TEST_MODE    2

#if (SI24R1_TEST_MODE == 1)
#define RF_SETUP_TX   (RF_SETUP_VALUE | RF_CONT_WAVE | RF_PLL_LOCK)  // CW + PLL_LOCK (Appendix C)
#else
#define RF_SETUP_TX   (RF_SETUP_VALUE)                               // modulated packet config
#endif

// ---- derived link switches ----
// Every function that ends up with no caller makes the linker print
// "L16 UNCALLED SEGMENT". Rather than silencing it, gate the unused code out
// at compile time so each mode links exactly what it uses:
//   mode 0 (demo) : RX path + TX path
//   mode 1 (CW)   : register access only
//   mode 2 (MOD)  : TX path only
#if (SI24R1_TEST_MODE == 0)
#define RF_LINK_RX  1
#else
#define RF_LINK_RX  0
#endif

#if (SI24R1_TEST_MODE != 1)
#define RF_LINK_TX  1
#else
#define RF_LINK_TX  0
#endif

// Mode 2 drives the transmitter itself: CE stays high and the TX FIFO is
// fed by hand, so the packet-by-packet path (SI24R1_TX_Mode +
// SI24R1_TxPacket) and the STATUS read it needs end up with no caller.
// Gate them out too, or the linker prints L16 for both.
#if (SI24R1_TEST_MODE == 0)
#define RF_LINK_TXPKT   1
#else
#define RF_LINK_TXPKT   0
#endif

//********************************************************************************************************************//
//                                        FUNCTION's PROTOTYPES                                                       //
//********************************************************************************************************************//
//SI24R1 API Functions
void SI24R1_Init(void); //SI24R1 Pin Init
u8 SI24R1_Write_Reg(u8 reg, u8 value); 
u8 SI24R1_Write_Buf(u8 reg, const u8 *pBuf, u8 bytes);
u8 SI24R1_Read_Reg(u8 reg);
u8 SI24R1_Read_Buf(u8 reg, u8 *pBuf, u8 bytes);

void SI24R1_RX_Mode(void);
void SI24R1_TX_Mode(void);
u8 SI24R1_RxPacket(u8 *rxbuf);
u8 SI24R1_TxPacket(u8 *txbuf);
void SI24R1_CW_Mode(void);                            // unmodulated carrier, Appendix C
void SI24R1_Continuous_Tx_Test(u8 *txbuf, u16 count); // continuous modulated TX

//********************************************************************************************************************//
#endif

