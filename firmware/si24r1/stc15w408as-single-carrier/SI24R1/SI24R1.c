#include "SI24R1.h"
#include "delay.h"

#if (RF_LINK_RX || RF_LINK_TX)
u8 code TX_ADDRESS[TX_ADR_WIDTH] = {0x0A,0x01,0x07,0x0E,0x01};  // 定义一个静态发送地址
#endif


static u8 SPI_RW(u8 byte)
{
	u8 bit_ctr;
	for(bit_ctr=0; bit_ctr<8; bit_ctr++)
	{
		if(byte & 0x80)
			MOSI = 1;
		else
			MOSI = 0;																 
		byte = (byte << 1);                      
		SCK = 1;                                   
		byte |= MISO;                             
		SCK = 0;                                
	}
	return(byte);                              
}


/********************************************************
函数功能：SI24R1引脚初始化                
入口参数：无
返回  值：无
*********************************************************/
void SI24R1_Init(void)
{
	SCK = 0; 													//SPI时钟线拉低
	CSN = 1;				
	CE 	= 0;				
	IRQ = 1;
}


/********************************************************
函数功能：写寄存器的值（单字节）                
入口参数：reg:寄存器映射地址（格式：WRITE_REG｜reg）
					value:寄存器的值
返回  值：状态寄存器的值
*********************************************************/
u8 SI24R1_Write_Reg(u8 reg, u8 value)
{
	u8 status;

	CSN = 0;                 
	status = SPI_RW(reg);				
	SPI_RW(value);
	CSN = 1;  
	
	return(status);
}


#if RF_LINK_TX
/********************************************************
函数功能：写寄存器的值（多字节）                  
入口参数：reg:寄存器映射地址（格式：WRITE_REG｜reg）
					pBuf:写数据首地址
					bytes:写数据字节数
返回  值：状态寄存器的值
*********************************************************/
u8 SI24R1_Write_Buf(u8 reg, const u8 *pBuf, u8 bytes)
{
	u8 status,byte_ctr;

  CSN = 0;                                  			
  status = SPI_RW(reg);                          
  for(byte_ctr=0; byte_ctr<bytes; byte_ctr++)     
    SPI_RW(*pBuf++);
  CSN = 1;                                      	

  return(status);       
}							  					   
#endif


#if (RF_LINK_RX || RF_LINK_TXPKT)
/********************************************************
函数功能：读取寄存器的值（单字节）                  
入口参数：reg:寄存器映射地址（格式：READ_REG｜reg）
返回  值：寄存器值
*********************************************************/
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


#if RF_LINK_RX
/********************************************************
函数功能：读取寄存器的值（多字节）                  
入口参数：reg:寄存器映射地址（READ_REG｜reg）
					pBuf:接收缓冲区的首地址
					bytes:读取字节数
返回  值：状态寄存器的值
*********************************************************/
u8 SI24R1_Read_Buf(u8 reg, u8 *pBuf, u8 bytes)
{
	u8 status,byte_ctr;

  CSN = 0;                                        
  status = SPI_RW(reg);                           
  for(byte_ctr=0;byte_ctr<bytes;byte_ctr++)
    pBuf[byte_ctr] = SPI_RW(0);                   //读取数据，低字节在前
  CSN = 1;                                        

  return(status);    
}
#endif


#if RF_LINK_RX
/********************************************************
函数功能：SI24R1接收模式初始化                      
入口参数：无
返回  值：无
*********************************************************/
void SI24R1_RX_Mode(void)
{
	CE = 0;
	SI24R1_Write_Buf(WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH);	// 接收设备接收通道0使用和发送设备相同的发送地址
	SI24R1_Write_Reg(WRITE_REG + EN_AA, 0x01);               						// 使能接收通道0自动应答
	SI24R1_Write_Reg(WRITE_REG + EN_RXADDR, 0x01);           						// 使能接收通道0
	SI24R1_Write_Reg(WRITE_REG + RF_CH, RF_CH_SEL);            // 2400 + RF_CH_SEL

	SI24R1_Write_Reg(WRITE_REG + RX_PW_P0, TX_PLOAD_WIDTH);  						// 接收通道0选择和发送通道相同有效数据宽度
	SI24R1_Write_Reg(WRITE_REG + RF_SETUP, RF_SETUP_VALUE);            						// rate + TX power, see RF_SETUP_VALUE / RF_SETUP_TX
	SI24R1_Write_Reg(WRITE_REG + CONFIG, 0x0f);              						// CRC使能，16位CRC校验，上电，接收模式
	SI24R1_Write_Reg(WRITE_REG + STATUS, 0xff);  												//清除所有的中断标志位
	CE = 1;                                            									// 拉高CE启动接收设备
}						
#endif


#if RF_LINK_TXPKT
/********************************************************
函数功能：SI24R1发送模式初始化                      
入口参数：无
返回  值：无
*********************************************************/
void SI24R1_TX_Mode(void)
{
	CE = 0;
	SI24R1_Write_Buf(WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);     // 写入发送地址
	SI24R1_Write_Buf(WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH);  // 为了应答接收设备，接收通道0地址和发送地址相同

	SI24R1_Write_Reg(WRITE_REG + EN_AA, 0x01);       											// 使能接收通道0自动应答
	SI24R1_Write_Reg(WRITE_REG + EN_RXADDR, 0x01);   											// 使能接收通道0
	SI24R1_Write_Reg(WRITE_REG + SETUP_RETR, 0x0a);  											// 自动重发延时等待250us+86us，自动重发10次
	SI24R1_Write_Reg(WRITE_REG + RF_SETUP, RF_SETUP_TX);    											// rate + TX power, see RF_SETUP_VALUE / RF_SETUP_TX
	SI24R1_Write_Reg(WRITE_REG + RF_CH, RF_CH_SEL);            // 2400 + RF_CH_SEL

	SI24R1_Write_Reg(WRITE_REG + CONFIG, 0x0e);      											// CRC使能，16位CRC校验，上电
	//CE = 1;
}
#endif


#if RF_LINK_RX
/********************************************************
函数功能：读取接收数据                       
入口参数：rxbuf:接收数据存放首地址
返回  值：0:接收到数据
          1:没有接收到数据
*********************************************************/
u8 SI24R1_RxPacket(u8 *rxbuf)
{
	u8 state;
	state = SI24R1_Read_Reg(STATUS);  			                 //读取状态寄存器的值    	  
	SI24R1_Write_Reg(WRITE_REG+STATUS,state);               //清除RX_DS中断标志

	if(state & RX_DR)								                           //接收到数据
	{
		SI24R1_Read_Buf(RD_RX_PLOAD,rxbuf,TX_PLOAD_WIDTH);     //读取数据
		SI24R1_Write_Reg(FLUSH_RX,0xff);					              //清除RX FIFO寄存器
		return 0; 
	}	   
	return 1;                                                   //没收到任何数据
}
#endif


#if RF_LINK_TXPKT
/********************************************************
函数功能：发送一个数据包                      
入口参数：txbuf:要发送的数据
返回  值：0x10:达到最大重发次数，发送失败 
          0x20:发送成功            
          0xff:发送失败                  
*********************************************************/
u8 SI24R1_TxPacket(u8 *txbuf)
{
	u8 state;
	CE=0;																										  //CE拉低，使能SI24R1配置
  SI24R1_Write_Buf(WR_TX_PLOAD, txbuf, TX_PLOAD_WIDTH);	    //写数据到TX FIFO,32个字节
 	CE=1;																										  //CE置高，使能发送	   
	
	while(IRQ == 1);																				  //等待发送完成
	state = SI24R1_Read_Reg(STATUS);  											  //读取状态寄存器的值	   
	SI24R1_Write_Reg(WRITE_REG+STATUS, state); 								//清除TX_DS或MAX_RT中断标志
	if(state&MAX_RT)																			    //达到最大重发次数
	{
		SI24R1_Write_Reg(FLUSH_TX,0xff);										    //清除TX FIFO寄存器 
		return MAX_RT; 
	}
	if(state&TX_DS)																			      //发送完成
	{
		return TX_DS;
	}
	return 0XFF;																						  //发送失败
}
#endif


#if (SI24R1_TEST_MODE == 1)
/********************************************************
函数功能：恒载波（未调制单载波）模式                      

          严格按 nRF24L01+ 规格书 Appendix C：
          CONFIG: PWR_UP=1, PRIM_RX=0
          等待 1.5ms (PWR_UP -> standby)
          RF_SETUP: CONT_WAVE=1, PLL_LOCK=1
          设置 RF_CH (2400 + RF_CH_SEL = 2475 MHz)
          CE 保持高电平，载波需要多久就保持多久
入口参数：无
返回  值：无
说明   ：本函数不返回，进入后载波持续输出
*********************************************************/
void SI24R1_CW_Mode(void)
{
	CE = 0;                                               // 配置期间拉低 CE
	SI24R1_Write_Reg(WRITE_REG + CONFIG, 0x0e);           // CRC使能，16位CRC校验，上电，发送模式
	delay_ms(2);                                          // 等待 PWR_UP -> standby (>=1.5ms)
	SI24R1_Write_Reg(WRITE_REG + RF_SETUP, RF_SETUP_TX);  // CONT_WAVE=1 + PLL_LOCK=1
	SI24R1_Write_Reg(WRITE_REG + RF_CH, RF_CH_SEL);            // 2400 + RF_CH_SEL = 2475 MHz

	CE = 1;                                               // 保持高电平，载波持续输出
}
#endif

#if (SI24R1_TEST_MODE == 2)
/********************************************************
函数功能：连续调制载波，用于占用带宽 / 6dB带宽 / PSD 测试

          认证测量要求发射机"连续发射"，逐个调用 SI24R1_TxPacket() 做不到：
          它每个包都要把 CE 拉低再拉高（每包多 130us 的 standby->TX 切换），
          再叠加 SETUP_RETR=0x0a 的 10 次自动重发和包间 delay，
          空口实际是一串几百 us 周期的通断突发。扫频式频谱仪抓不稳这种信号，
          迹线会发毛、99% OBW 和 -xdB 带宽的读数都会失真。

          这里改成 nRF24L01+ 手册 Appendix C 的思路：
            - EN_AA / SETUP_RETR 清零，不等 ACK、不自动重发
            - CE 保持高电平，发射机不再回 standby
            - CE 高电平期间持续往 TX FIFO 喂新的伪随机载荷，载波始终在调制
          注意：载荷不能是全 0x00 / 0xFF，否则退化成近乎未调制的单线，
                测出来的占用带宽会塌缩。
                另：2 Mbps 档每包只有 ~165us，而 IO 模拟 SPI 写一次 33 字节
                约 190us，喂不快会出现包间空档；该档建议先用 1 Mbps 测。
入口参数：txbuf: 载荷缓冲区(32 字节，内容由本函数覆盖)；count: 包数，0=无限
返回  值：无
*********************************************************/
void SI24R1_Continuous_Tx_Test(u8 *txbuf, u16 count)
{
	u16 i, j;
	u8  seed = 0x5a;

	CE = 0;
	SI24R1_Write_Reg(WRITE_REG + CONFIG, 0x0e);          // PWR_UP=1, PRIM_RX=0, 16 位 CRC
	delay_ms(2);                                          // 等 PWR_UP -> standby (>=1.5ms)

	SI24R1_Write_Buf(WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);
	SI24R1_Write_Reg(WRITE_REG + EN_AA, 0x00);            // 关自动应答
	SI24R1_Write_Reg(WRITE_REG + SETUP_RETR, 0x00);       // 关自动重发
	SI24R1_Write_Reg(WRITE_REG + RF_SETUP, RF_SETUP_TX);  // 速率 + 发射功率
	SI24R1_Write_Reg(WRITE_REG + RF_CH, RF_CH_SEL);            // 2400 + RF_CH_SEL = 2475 MHz

	SI24R1_Write_Reg(WRITE_REG + STATUS, 0xff);           // 清中断标志

	CE = 1;                                               // 发射机启动，之后不再翻转
	for(i = 0; ; i++)
	{
		for(j = 0; j < TX_PLOAD_WIDTH; j++)               // 伪随机载荷，保证持续调制
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

