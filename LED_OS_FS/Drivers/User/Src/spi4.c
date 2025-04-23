#include "main.h"


//以下是SPI模块的初始化代码，配置成主机模式 						  
//SPI口初始化
//这里针是对SPI4的初始化
//PE2->SPI4_SCK PE5->SPI4_MISO PE6->SPI4_MOSI
void MAG0_SPI4_Configuration(void)
{	 
	uint32_t tempreg=0;
	RCC->AHB4ENR|=1<<4;					//使能PORTE时钟  20210208
	RCC->APB2ENR|=1<<13;				//SPI4时钟使能 
	//配置SPI的时钟源
//	RCC->D2CCIP1R&=~(7<<12);		//SPI123SEL[2:0]=0,清除原来的设置
//	RCC->D2CCIP1R|=0<<12;				//SPI123SEL[2:0]=1,选择pll1_q_ck作为SPI1/2/3的时钟源,一般为200Mhz
	RCC->D2CCIP1R&=~(7<<16);		//SPI45SEL[2:0]=0,清除原来的设置
	RCC->D2CCIP1R|=0<<16;				//SPI45SEL[2:0]=1,选择pll1_q_ck作为SPI1/2/3的时钟源,一般为200Mhz
															//即:spi_ker_ck=200Mhz 
	//这里只针对SPI口初始化
//	RCC->APB1LRSTR|=1<<14;			//复位SPI2
//	RCC->APB1LRSTR&=~(1<<14);		//停止复位SPI2
	RCC->APB2RSTR|=1<<13;			//复位SPI4
	RCC->APB2RSTR&=~(1<<13);		//停止复位SPI4
	
	SPI4->CR1|=1<<12;						//SSI=1,设置内部SS信号为高电平	
	SPI4->CFG1=6<<28;						//MBR[2:0]=7,设置spi_ker_ck为256分频.    3:1/16    4:1/32    5: 1/64  6: 1/128
	SPI4->CFG1|=7<<0;						//DSIZE[4:0]=7,设置SPI帧格式为8位,即字节传输
	tempreg=(uint32_t)1<<31;		//AFCNTR=1,SPI保持对IO口的控制
	tempreg|=0<<29;							//SSOE=0,禁止硬件NSS输出
	tempreg|=1<<26;							//SSM=1,软件管理NSS脚
	tempreg|=1<<25;							//CPOL=1,空闲状态下,SCK为高电平
	tempreg|=1<<24;							//CPHA=1,数据采样从第2个时间边沿开始
	tempreg|=0<<23;							//LSBFRST=0,MSB先传输
	tempreg|=1<<22;							//MASTER=1,主机模式
	tempreg|=0<<19;							//SP[2:0]=0,摩托罗拉格式
	tempreg|=0<<17;							//COMM[1:0]=0,全双工通信
	SPI4->CFG2=tempreg;					//设置CFG2寄存器	
	SPI4->I2SCFGR&=~(1<<0);			//选择SPI模式
	SPI4->CR1|=1<<0;						//SPE=1,使能SPI4

  GPIO_InitTypeDef hspi;
  hspi.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
  hspi.Mode = GPIO_MODE_AF_PP;
  hspi.Pull = GPIO_PULLUP;
  hspi.Speed = GPIO_SPEED_FREQ_HIGH;
  hspi.Alternate = GPIO_AF5_SPI1;
  // GPIO配置
  HAL_GPIO_Init(GPIOA, &hspi);
}   

//SPI4速度设置函数
//SpeedSet:0~7
//SPI速度=spi_ker_ck/2^(SpeedSet+1)
//spi_ker_ck我们选择来自pll1_q_ck,为200Mhz
void SPI4_SetSpeed(uint8_t SpeedSet)
{
	SpeedSet&=0X07;							//限制范围
 	SPI4->CR1&=~(1<<0); 				//SPE=0,SPI设备失能
	SPI4->CFG1&=~(7<<28); 			//MBR[2:0]=0,清除原来的分频设置
	SPI4->CFG1|=(uint32_t)SpeedSet<<28;	//MBR[2:0]=SpeedSet,设置SPI4速度  
	SPI4->CR1|=1<<0; 						//SPE=1,SPI设备使能	 	  
} 

//SPI4 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
uint8_t SPI4_ReadWriteByte(uint8_t TxData)
{			 	   
	uint8_t RxData=0;	
	SPI4->CR1|=1<<0;							//SPE=1,使能SPI4
	SPI4->CR1|=1<<9;  						//CSTART=1,启动传输
	
	while((SPI4->SR&1<<1)==0);		//等待发送区空 
	*(volatile uint8_t *)&SPI4->TXDR=TxData;		//发送一个byte,以传输长度访问TXDR寄存器   
	while((SPI4->SR&1<<0)==0);		//等待接收完一个byte  
	RxData=*(volatile uint8_t *)&SPI4->RXDR;		//接收一个byte,以传输长度访问RXDR寄存器	
	
	SPI4->IFCR|=3<<3;							//EOTC和TXTFC置1,清除EOT和TXTFC位 
	SPI4->CR1&=~(1<<0);						//SPE=0,关闭SPI4,会执行状态机复位/FIFO重置等操作
	return RxData;								//返回收到的数据
}






