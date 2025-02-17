#include "spi.h"


void spi_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	
	
	GPIO_InitStructure.GPIO_Pin = MISO_PIN | MOSI_PIN | SCK_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOPORT, &GPIO_InitStructure);
	
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;//48/2 = 24M 24/4 = 6Mhz
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;//
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPIPORT, &SPI_InitStructure);
	
	SPI_I2S_DMACmd(SPI2,SPI_I2S_DMAReq_Tx, ENABLE);
	SPI_Cmd(SPIPORT, ENABLE);
	
}


//设置spi分频系数
//BIDIMode, BIDIOE, RxONLY, SSM, SSI, LSBFirst, BR, MSTR, CPOL and CPHA bits
void set_spix_baudrate(uint16_t SPI_BaudRatePrescaler)
{
	//设置CR1寄存器的相关位即可
	assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler)); //断言 确定参数是否在正确范围内
	SPIPORT->CR1 &= 0xFFC7;//只需要修改bit3-bit5
	SPIPORT->CR1 |= SPI_BaudRatePrescaler;
	SPI_Cmd(SPIPORT, ENABLE);
}


u8 spi_read_write_byte(u8 TxData)
{		
	u8 retry=0;				 	
	while (SPI_I2S_GetFlagStatus(SPIPORT, SPI_I2S_FLAG_TXE) == RESET) //检查指定的SPI标志位设置与否:发送缓存空标志位
		{
		retry++;
		if(retry>200)return 0;
		}			  
	SPI_I2S_SendData(SPIPORT, TxData); //通过外设SPIx发送一个数据
//	retry=0;

//	while (SPI_I2S_GetFlagStatus(SPIPORT, SPI_I2S_FLAG_RXNE) == RESET) //检查指定的SPI标志位设置与否:接受缓存非空标志位
//		{
//		retry++;
//		if(retry>200)return 0;
//		}	  						    
//	return SPI_I2S_ReceiveData(SPIPORT); //返回通过SPIx最近接收的数据
}






void SPI1_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_SPI1, ENABLE);
	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOA,GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7); //PA5.6.7上拉
	
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI1, &SPI_InitStructure);
	
	
	SPI_Cmd(SPI1, ENABLE);
}


//设置spi分频系数
//BIDIMode, BIDIOE, RxONLY, SSM, SSI, LSBFirst, BR, MSTR, CPOL and CPHA bits
void SPI1_set_baudrate(uint16_t SPI_BaudRatePrescaler)
{
	//设置CR1寄存器的相关位即可
	assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler)); //断言 确定参数是否在正确范围内
	SPI1->CR1 &= 0xFFC7;//只需要修改bit3-bit5
	SPI1->CR1 |= SPI_BaudRatePrescaler;
	SPI_Cmd(SPI1, ENABLE);
}



u8 SPI1_read_write_byte(u8 TxData)
{		
	u8 retry=0;				 	
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) //检查指定的SPI标志位设置与否:发送缓存空标志位
		{
		retry++;
		if(retry>200)return 0;
		}			  
	SPI_I2S_SendData(SPI1, TxData); //通过外设SPIx发送一个数据
	retry=0;

	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET) //检查指定的SPI标志位设置与否:接受缓存非空标志位
		{
		retry++;
		if(retry>200)return 0;
		}	  						    
	return SPI_I2S_ReceiveData(SPI1); //返回通过SPIx最近接收的数据
}
