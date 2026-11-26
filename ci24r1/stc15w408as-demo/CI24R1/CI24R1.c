//版本 v0.0
#include "CI24R1.h"

u8 code TR_ADDRESS[TR_ADR_WIDTH] = {0x0A,0x01,0x07,0x0E,0x10};  // 定义一个静态接收/发送地址
u8  TX_REG18_2M[4]={0x82,0x79,0x80,0x00};
u8  TX_REG18_1M[4]={0x82,0x71,0x82,0x00};
u8  TX_REG18_250k[4]={0x82,0x71,0x82,0x00};
u8 TX_REG18[4]={0};
/*************************************************************************************************
函数名称：SPI1_WriteByte
功    能：SPI1写一个字节
参    数：src,待写入值
返 回 值：无
***************************************************************************************************/
void	SPI1_WriteByte(u8	src)
{
		u8 i=0;
		SCK = 0;
		for(i=0;i<8;i++)
	{
			SCK = 0;
			if(src&0x80)
			{
				MOSI = 1;
			}
			else
			{	
				MOSI = 0;
			}
			SCK = 1;
			src=src<<1;
	}
		SCK = 0;
}
/*************************************************************************************************
函数名称：SPI1_ReadByte
功    能：SPI1读一个字节
参    数：无
返 回 值：RxData
***************************************************************************************************/
u8	SPI1_ReadByte(void)
{
	u8 i=0,RxData;
		SCK = 0;
	
	for(i=0;i<8;i++)
	{
		RxData=RxData<<1;
		SCK = 1;

		if(MOSI == 1)
		{
		RxData|=0x01;
		}
		else
		{
			RxData&=0xfe;
		}
		SCK = 0;
	}
	
	SCK = 0;
	//CSN = 1;
	
	return RxData;
}

/********************************************************
函数功能：CI24R1引脚初始化                
入口参数：无
返 回 值：无
*********************************************************/
void CI24R1_Init(void)
{	
	CSN = 0;
	CSN = 1;
	SCK = 0;
	CSN = 0;
  SPI1_WriteByte(SELSPI);
  CSN = 1;		
	
	//CI24R1_Write_Reg(WRITE_REG + CONFIG, 0x5c);      											   // 配置基本工作模式的参数，PWR_UP,EN_CRC,16BIT,发射模式
	//CI24R1_Write_Reg(WRITE_REG + CONFIG, 0x5e);      											   // 配置基本工作模式的参数，PWR_UP,EN_CRC,16BIT,发射模式
}


/********************************************************
函数功能：写寄存器的值（单字节）                
入口参数：reg:寄存器映射地址（格式：WRITE_REG｜reg）
					value:寄存器的值
返 回 值：0
*********************************************************/
u8 CI24R1_Write_Reg(u8 reg, u8 value)
{
//	u8 status;

	CSN = 0;                 
	SPI1_WriteByte(reg);				
	SPI1_WriteByte(value);
	CSN = 1;  
	
	return 0;
}


/********************************************************
函数功能：写寄存器的值（多字节）                  
入口参数：reg:寄存器映射地址（格式：WRITE_REG｜reg）
					pBuf:写数据首地址
					bytes:写数据字节数
返 回 值：0
*********************************************************/
u8 CI24R1_Write_Buf(u8 reg, const u8 *pBuf, u8 bytes)
{
	u8 byte_ctr;

  CSN = 0;                                  			
    SPI1_WriteByte(reg);                          
  for(byte_ctr=0; byte_ctr<bytes; byte_ctr++)     
    SPI1_WriteByte(*pBuf++);
  CSN = 1;                                      	

  return 0;       
}							  					   


/********************************************************
函数功能：读取寄存器的值（单字节）                  
入口参数：reg:寄存器映射地址（格式：READ_REG｜reg）
返 回 值：寄存器值
*********************************************************/
u8 CI24R1_Read_Reg(u8 reg)
{
 	u8 value;

	CSN = 0;    
	SPI1_WriteByte(reg);			
	value = SPI1_ReadByte();
	CSN = 1;              

	return(value);
}


/********************************************************
函数功能：读取寄存器的值（多字节）                  
入口参数：reg:寄存器映射地址（READ_REG｜reg）
					pBuf:接收缓冲区的首地址
					bytes:读取字节数
返 回 值：0
*********************************************************/
u8 CI24R1_Read_Buf(u8 reg, u8 *pBuf, u8 bytes)
{
	u8 byte_ctr;

  CSN = 0;                                        
    SPI1_WriteByte(reg);                           
  for(byte_ctr=0;byte_ctr<bytes;byte_ctr++)
    pBuf[byte_ctr] = SPI1_ReadByte();                                 //读取数据直接写入数组中，低字节在前
  CSN = 1;                                        

  return 0;    
}


/********************************************************
函数功能：CI24R1接收模式初始化                      
入口参数：无
返 回 值：无
*********************************************************/
void CI24R1_RX_Mode(void)
{
	
	CI24R1_Write_Reg(CE_OFF,0xff);                                      // CE = 0;
	CI24R1_Write_Buf(WRITE_REG + RX_ADDR_P0, TR_ADDRESS, TR_ADR_WIDTH);	// 接收设备接收通道0使用和发送设备相同的发送地址
    CI24R1_Write_Reg(WRITE_REG + SETUP_AW, 0x03);                       // 设置接收地址宽度为4个字节；
	CI24R1_Write_Reg(WRITE_REG + EN_AA, 0x01);               						// 使能接收通道0自动应答
	CI24R1_Write_Reg(WRITE_REG + EN_RXADDR, 0x01);           						// 使能接收通道0
	CI24R1_Write_Reg(WRITE_REG + FEATURE, 0x01);

//	Ci24R1_Reg18_change();
	
	CI24R1_Write_Reg(WRITE_REG + RF_CH, 80);                 					// 设置RF频率为2480MHz
	CI24R1_Write_Reg(WRITE_REG + RX_PW_P0, TR_PLOAD_WIDTH);  						// 接收通道0选择和发送通道相同有效数据宽度
	CI24R1_Write_Reg(WRITE_REG + RF_SETUP, 0x0D);            					// 数据传输率2Mbps，发射功率9dBm  （注：数据传输率2Mbps 16进制 低位+8）
	//CI24R1_Write_Reg(WRITE_REG + RF_SETUP, 0x25);            						// 数据传输率250Kbps，发射功率9dBm  （注：数据传输率250Kbps 16进制 低位+8）
	CI24R1_Write_Reg(WRITE_REG + CONFIG, 0x0f);              						// 配置基本工作模式的参数，PWR_UP,EN_CRC,16BIT,接收模式
	
//	/*模拟电容*/
	CI24R1_Write_Reg(WRITE_REG + 0x02,0X01);
	CI24R1_Write_Reg(WRITE_REG + 0x01,0X81);
	CI24R1_Write_Reg(WRITE_REG + 0x0f,0XA0);                            // 目前是15pf

	CI24R1_Write_Reg(WRITE_REG + STATUS, 0xff);  			            // 清除所有的中断标志位
	CI24R1_Write_Reg(CE_ON,0xff);                                       // CE = 1;CE拉高，进入接收模式
}						

/********************************************************
函数功能：CI24R1发送模式初始化                      
入口参数：无
返 回 值：无
*********************************************************/
void CI24R1_TX_Mode(void)
{
	
	CI24R1_Write_Reg(CE_OFF,0xff);                                           // CE = 0;                                         
	CI24R1_Write_Reg(WRITE_REG + SETUP_AW, 0X03);                            // 设置接收地址宽度为4个字节
	CI24R1_Write_Buf(WRITE_REG + TX_ADDR, (u8 *)TR_ADDRESS, TR_ADR_WIDTH);   // 写入发送地址
	CI24R1_Write_Buf(WRITE_REG + RX_ADDR_P0, TR_ADDRESS, TR_ADR_WIDTH);      // 写入发送地址

	CI24R1_Write_Reg(WRITE_REG + EN_AA, 0x01);       											   // 为了应答接收设备，接收通道0地址和发送地址相同
	CI24R1_Write_Reg(WRITE_REG + EN_RXADDR, 0x01);   											   // 使能接收通道0自动应答
	CI24R1_Write_Reg(WRITE_REG + FEATURE, 0x01);                             // 使能接收通道0
	CI24R1_Write_Reg(WRITE_REG + SETUP_RETR, 0x0a);  											   // 自动重发延时等待250us+86us，自动重发10次
	
//	/*模拟电容*/
	CI24R1_Write_Reg(WRITE_REG + 0x02,0X01);
	CI24R1_Write_Reg(WRITE_REG + 0x01,0X81);
	CI24R1_Write_Reg(WRITE_REG + 0x0f,0XA0);                                 // 目前是15pf
	
//	Ci24R1_Reg18_change();
	
	CI24R1_Write_Reg(WRITE_REG + RF_CH, 80);         											  // 设置RF频率为2480MHz	
	CI24R1_Write_Reg(WRITE_REG + RF_SETUP, 0x0D);            					// 数据传输率2Mbps，发射功率9dBm  （注：数据传输率2Mbps 16进制 低位+8）
	//CI24R1_Write_Reg(WRITE_REG + RF_SETUP, 0x25);            						// 数据传输率250Kbps，发射功率9dBm  （注：数据传输率250Kbps 16进制 低位+8）
	CI24R1_Write_Reg(WRITE_REG + CONFIG, 0x5e);      											   // 配置基本工作模式的参数，PWR_UP,EN_CRC,16BIT,发射模式
	//CE = 1;
}

/********************************************************
函数功能：读取接收数据                       
入口参数：rxbuf:接收数据存放首地址
返 回 值：0:接收到数据
          1:没有接收到数据
*********************************************************/
u8 CI24R1_RxPacket(u8 *rxbuf)
{
	u8 status;
/*****************************IRQ模式接收*********************************/	
//  CSN = 0;
//	SPI1_WriteByte(SELIRQ);
//	CSN = 1;
//	CI24R1_Read_Reg(WRITE_REG+STATUS);
// 
//	delay_ms(20);                                        
//	if (IRQ == 1)                                           //一直等待IRQ引脚被拉低（CI24R1的IRQ低电平有效）	
//		return 1;
/**************************************************************************/		

	CSN = 0;
	SPI1_WriteByte(SELSPI);
	CSN = 1;
	status=CI24R1_Read_Reg(STATUS);
	CI24R1_Write_Reg(WRITE_REG+STATUS,status);
	if(status&RX_DR)
	{
		while((status&RX_P_NO)!=0x0e)                         //判断RX_FIFO是否为空，不为空继续读
		{
			CI24R1_Read_Buf(RD_RX_PLOAD,rxbuf,TR_PLOAD_WIDTH);
			status=CI24R1_Read_Reg(WRITE_REG+STATUS);         	//继续读状态寄存器判断RX_FIFO标志位
		}	
		CI24R1_Write_Reg(FLUSH_RX,0xff);							        //清空RX_FIFO
		return 0;
	}
	 return 1;

}


/********************************************************
函数功能：发送一个数据包                      
入口参数：txbuf:要发送的数据
返 回 值：0x10:达到最大重发次数，发送失败 
          0x20:发送成功            
          0xff:发送失败                  
*********************************************************/
u8 CI24R1_TxPacket(u8 *txbuf)
{
	u8 state;
	
	//u8 User_Data[5] = {0x55, 0x02, 0x03, 0x04, 0x05};
	
	CI24R1_Write_Reg(CE_OFF,0xff);//CE=0;												  //CE拉低，使能CI24R1配置
  //CI24R1_Write_Reg(FLUSH_TX,0XFF);                            //清除TX FIFO的值
  CI24R1_Write_Buf(W_TX_PAYLOAD_NOACK, txbuf, TR_PLOAD_WIDTH);	//写数据到TX FIFO,32个字节
 	CI24R1_Write_Reg(CE_ON,0xff);//CE=1;				                  //CE拉高，CI24R1开始发射														
	
	delay_ms(20);
	
	CSN = 0;
	SPI1_WriteByte(SELIRQ);
	CSN = 1;
	
	CI24R1_Read_Reg(READ_REG+STATUS);
	
	while(IRQ != 0);																				      //一直等待IRQ引脚被拉低（CI24R1的IRQ低电平有效）
	
	CSN = 0;
	SPI1_WriteByte(SELSPI);
	CSN = 1;
	
	state = CI24R1_Read_Reg(READ_REG+STATUS);  								   //读取状态寄存器的值	   
	CI24R1_Write_Reg(WRITE_REG+STATUS, state); 								   //清除TX_DS或MAX_RT中断标志
	if(state&MAX_RT)																			       //达到最大重发次数
	{
		CI24R1_Write_Reg(FLUSH_TX,0xff);										       //清除TX FIFO寄存器 
		return MAX_RT; 
	}
	if(state&TX_DS)																			         //发送完成
	{
		return TX_DS;
	}
	return 0XFF;																						     //发送失败
}

//void Ci24R1_Reg18_change()
//{
//	CI24R1_Write_Reg(CE_OFF,0xff);	
//	CI24R1_Write_Reg(0x50,0xa5);

//	CI24R1_Read_Buf(WRITE_REG+BandWidth_TX,(u8 *)TX_REG18,4);
//	CI24R1_Write_Buf(WRITE_REG+BandWidth_TX,(u8 *)TX_REG18_250k,4);  //增加调制带宽（250kpbs/1Mpbs的修改相同，2M的不同）
//	CI24R1_Write_Reg(CE_OFF,0xff);
//	CI24R1_Write_Reg(0x50,0xa5);

//}