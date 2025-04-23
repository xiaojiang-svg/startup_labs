#include "main.h"


//������SPIģ��ĳ�ʼ�����룬���ó�����ģʽ 						  
//SPI�ڳ�ʼ��
//�������Ƕ�SPI4�ĳ�ʼ��
//PE2->SPI4_SCK PE5->SPI4_MISO PE6->SPI4_MOSI
void MAG0_SPI4_Configuration(void)
{	 
	uint32_t tempreg=0;
	RCC->AHB4ENR|=1<<4;					//ʹ��PORTEʱ��  20210208
	RCC->APB2ENR|=1<<13;				//SPI4ʱ��ʹ�� 
	//����SPI��ʱ��Դ
//	RCC->D2CCIP1R&=~(7<<12);		//SPI123SEL[2:0]=0,���ԭ��������
//	RCC->D2CCIP1R|=0<<12;				//SPI123SEL[2:0]=1,ѡ��pll1_q_ck��ΪSPI1/2/3��ʱ��Դ,һ��Ϊ200Mhz
	RCC->D2CCIP1R&=~(7<<16);		//SPI45SEL[2:0]=0,���ԭ��������
	RCC->D2CCIP1R|=0<<16;				//SPI45SEL[2:0]=1,ѡ��pll1_q_ck��ΪSPI1/2/3��ʱ��Դ,һ��Ϊ200Mhz
															//��:spi_ker_ck=200Mhz 
	//����ֻ���SPI�ڳ�ʼ��
//	RCC->APB1LRSTR|=1<<14;			//��λSPI2
//	RCC->APB1LRSTR&=~(1<<14);		//ֹͣ��λSPI2
	RCC->APB2RSTR|=1<<13;			//��λSPI4
	RCC->APB2RSTR&=~(1<<13);		//ֹͣ��λSPI4
	
	SPI4->CR1|=1<<12;						//SSI=1,�����ڲ�SS�ź�Ϊ�ߵ�ƽ	
	SPI4->CFG1=6<<28;						//MBR[2:0]=7,����spi_ker_ckΪ256��Ƶ.    3:1/16    4:1/32    5: 1/64  6: 1/128
	SPI4->CFG1|=7<<0;						//DSIZE[4:0]=7,����SPI֡��ʽΪ8λ,���ֽڴ���
	tempreg=(uint32_t)1<<31;		//AFCNTR=1,SPI���ֶ�IO�ڵĿ���
	tempreg|=0<<29;							//SSOE=0,��ֹӲ��NSS���
	tempreg|=1<<26;							//SSM=1,�������NSS��
	tempreg|=1<<25;							//CPOL=1,����״̬��,SCKΪ�ߵ�ƽ
	tempreg|=1<<24;							//CPHA=1,���ݲ����ӵ�2��ʱ����ؿ�ʼ
	tempreg|=0<<23;							//LSBFRST=0,MSB�ȴ���
	tempreg|=1<<22;							//MASTER=1,����ģʽ
	tempreg|=0<<19;							//SP[2:0]=0,Ħ��������ʽ
	tempreg|=0<<17;							//COMM[1:0]=0,ȫ˫��ͨ��
	SPI4->CFG2=tempreg;					//����CFG2�Ĵ���	
	SPI4->I2SCFGR&=~(1<<0);			//ѡ��SPIģʽ
	SPI4->CR1|=1<<0;						//SPE=1,ʹ��SPI4

  GPIO_InitTypeDef hspi;
  hspi.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
  hspi.Mode = GPIO_MODE_AF_PP;
  hspi.Pull = GPIO_PULLUP;
  hspi.Speed = GPIO_SPEED_FREQ_HIGH;
  hspi.Alternate = GPIO_AF5_SPI1;
  // GPIO����
  HAL_GPIO_Init(GPIOA, &hspi);
}   

//SPI4�ٶ����ú���
//SpeedSet:0~7
//SPI�ٶ�=spi_ker_ck/2^(SpeedSet+1)
//spi_ker_ck����ѡ������pll1_q_ck,Ϊ200Mhz
void SPI4_SetSpeed(uint8_t SpeedSet)
{
	SpeedSet&=0X07;							//���Ʒ�Χ
 	SPI4->CR1&=~(1<<0); 				//SPE=0,SPI�豸ʧ��
	SPI4->CFG1&=~(7<<28); 			//MBR[2:0]=0,���ԭ���ķ�Ƶ����
	SPI4->CFG1|=(uint32_t)SpeedSet<<28;	//MBR[2:0]=SpeedSet,����SPI4�ٶ�  
	SPI4->CR1|=1<<0; 						//SPE=1,SPI�豸ʹ��	 	  
} 

//SPI4 ��дһ���ֽ�
//TxData:Ҫд����ֽ�
//����ֵ:��ȡ�����ֽ�
uint8_t SPI4_ReadWriteByte(uint8_t TxData)
{			 	   
	uint8_t RxData=0;	
	SPI4->CR1|=1<<0;							//SPE=1,ʹ��SPI4
	SPI4->CR1|=1<<9;  						//CSTART=1,��������
	
	while((SPI4->SR&1<<1)==0);		//�ȴ��������� 
	*(volatile uint8_t *)&SPI4->TXDR=TxData;		//����һ��byte,�Դ��䳤�ȷ���TXDR�Ĵ���   
	while((SPI4->SR&1<<0)==0);		//�ȴ�������һ��byte  
	RxData=*(volatile uint8_t *)&SPI4->RXDR;		//����һ��byte,�Դ��䳤�ȷ���RXDR�Ĵ���	
	
	SPI4->IFCR|=3<<3;							//EOTC��TXTFC��1,���EOT��TXTFCλ 
	SPI4->CR1&=~(1<<0);						//SPE=0,�ر�SPI4,��ִ��״̬����λ/FIFO���õȲ���
	return RxData;								//�����յ�������
}






