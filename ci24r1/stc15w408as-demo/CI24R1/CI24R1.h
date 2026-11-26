#ifndef __CI24R1__
#define __CI24R1__

#include "config.h"
#include "delay.h"

//SI24R1 PIN DEFINITION 
#define	CSN       P15             // Slave Select pin, (output to CSN)
#define	SCK       P12             // Serial Clock pin, (output)
#define	IRQ       P10             // Interrupt signal, from nRF24L01 (input)
#define	MOSI      P10             // Master Out, Slave In pin (output)

#define TR_ADR_WIDTH   5  				// 5字节宽度的发送/接收地
#define TR_PLOAD_WIDTH 32  				// 数据通道有效数据宽度

//********************************************************************************************************************//
//SPI(CI24R1) commands
#define READ_REG            0x00  // Define read command to register
#define WRITE_REG           0x20  // Define write command to register
#define RD_RX_PLOAD         0x61  // Define RX payload register address
#define WR_TX_PLOAD         0xA0  // Define TX payload register address
#define FLUSH_TX            0xE1  // Define flush TX register command
#define FLUSH_RX            0xE2  // Define flush RX register command
#define REUSE_TX_PL         0xE3  // Define reuse TX payload register command
#define R_RX_PL_WID     	  0x60  // Define Read RX payload width for the R_RX_PAYLOAD in the RX FIFO
#define W_ACK_PAYLOAD   	  0xA8	// Define Write Payload to be transmitted together with ACK packet on PIPExxx xxx valid in the range from 000 to 101)
#define W_TX_PAYLOAD_NOACK	0xB0	// Define Write TX Disables AUTOACK on this specific packet
#define NOP         		    0xFF  // Define No Operation, might be used to read status register
//======================================================================
//SPI commands (Ci24R1) (newly increased!!!!)
#define CE_ON         	    0x70  //打开CE  CE=1,CE_STATE=1
#define CE_OFF 					    0x71	//关闭CE  CE=0,CE_STATE=0
#define SELSPI					    0x74	//选择DATA引脚为SPI功能
#define SELIRQ					    0x75	//选择DATA引脚为输出IRQ值
//======================================================================
//********************************************************************************************************************//

// SPI(CI24R1) registers(addresses)
#define CONFIG              0x00  // 'Config' register address
#define EN_AA               0x01  // 'Enable Auto Acknowledgment' register address
#define EN_RXADDR           0x02  // 'Enabled RX addresses' register address
#define SETUP_AW            0x03  // 'Setup address width' register address
#define SETUP_RETR          0x04  // 'Setup Auto. Retrans' register address
#define RF_CH               0x05  // 'RF channel' register address
#define RF_SETUP            0x06  // 'RF setup' register address
#define STATUS           		0x07  // 'Status' register address
#define OBSERVE_TX      		0x08  // 'Observe TX' register address
#define RSSI            		0x09  // 'Received Signal Strength Indecator' register address
#define RX_ADDR_P0      		0x0A  // 'RX address pipe0' register address
#define RX_ADDR_P1      		0x0B  // 'RX address pipe1' register address
#define RX_ADDR_P2      		0x0C  // 'RX address pipe2' register address
#define RX_ADDR_P3      		0x0D  // 'RX address pipe3' register address
#define RX_ADDR_P4     		 	0x0E  // 'RX address pipe4' register address
#define RX_ADDR_P5      		0x0F  // 'RX address pipe5' register address
#define TX_ADDR         		0x10  // 'TX address' register address
#define RX_PW_P0        		0x11  // 'RX payload width, pipe0' register address
#define RX_PW_P1        		0x12  // 'RX payload width, pipe1' register address
#define RX_PW_P2        		0x13  // 'RX payload width, pipe2' register address
#define RX_PW_P3        		0x14  // 'RX payload width, pipe3' register address
#define RX_PW_P4        		0x15  // 'RX payload width, pipe4' register address
#define RX_PW_P5        		0x16  // 'RX payload width, pipe5' register address
#define FIFO_STATUS     		0x17  // 'FIFO Status Register' register address
#define BandWidth_TX    		0x18  
#define DYNPD           		0x1C  // 'Enable dynamic paload length' register address
#define FEATURE         		0x1D  //  Feature Register address
//********************************************************************************************************************//
// STATUS Register 
#define RX_DR								0x40  // RX FIFO has a value flag bit
#define TX_DS								0x20  // Transmitting end transmitting interrupt bit
#define MAX_RT							0x10  // 达到最大重发次数中断位
#define RX_P_NO				  		0X0E  // 接收FIFO为空标志
//********************************************************************************************************************//
//                                        FUNCTION's PROTOTYPES                                                       //
//********************************************************************************************************************//
//CI24R1 API Functions
void CI24R1_Init(void); //CI24R1 Pin Init
u8 CI24R1_Write_Reg(u8 reg, u8 value); 
u8 CI24R1_Write_Buf(u8 reg, const u8 *pBuf, u8 bytes);
u8 CI24R1_Read_Reg(u8 reg);
u8 CI24R1_Read_Buf(u8 reg, u8 *pBuf, u8 bytes);

void CI24R1_RX_Mode(void);
void CI24R1_TX_Mode(void);
u8 CI24R1_RxPacket(u8 *rxbuf);
u8 CI24R1_TxPacket(u8 *txbuf);

//void Ci24R1_Reg18_change();
//********************************************************************************************************************//
#endif

