#include "main.h"
			
extern int mainLoopFlg;
int32_t timer5counter;
//extern int32_t timeNow;


//定时器3中断服务程序	 		//TIM3 serves the main loop
// void TIM3_IRQHandler(void)
// { 		  
// 	static uint8_t led1sta=1;
// 	if(TIM3->SR&0X0001)		//溢出中断
// 	{
// //		LED1(led1sta^=1);	 
// 		mainLoopFlg = 1;
// 	}				   
// 	TIM3->SR&=~(1<<0);		//清除中断标志位 	    
// }


//定时器4中断服务程序	 
void TIM4_IRQHandler(void)
{ 		  
	static uint8_t led1sta=1;
	if(TIM4->SR&0X0001)		//溢出中断
	{
//		LED1(led1sta^=1);	 
	}				   
	TIM4->SR&=~(1<<0);		//清除中断标志位 	    
}

//定时器5中断服务程序	 
void TIM5_IRQHandler(void)
{ 		  
	static uint8_t led1sta=1;
	if(TIM5->SR&0X0001)		//溢出中断
	{
			timer5counter++;
	}				
//	int32_t temp_timeNow = 0;
//	temp_timeNow = TIM5->CNT;
//	timeNow = temp_timeNow;
	TIM5->SR&=~(1<<0);		//清除中断标志位 	    
}

int32_t getTim5Counter()
{
	uint64_t Now = 0;
	Now = TIM5->CNT;
	return Now;
}

//通用定时器3中断初始化
//TIM3的时钟来自APB1,当D2PPRE1≥2分频的时候
//TIM3的时钟为APB1时钟的2倍，而APB1为100M
//所以,TIM3的时钟频率为200Mhz
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
//这里使用的是定时器3!
void TIM3_Int_Init(uint16_t arr,uint16_t psc)
{
	RCC->APB1LENR|=1<<1;							//TIM3时钟使能 		手册P438 
	//如果开启TIM3时钟后,直接开始配置TIM3,则会有问题
	while((RCC->APB1LENR&(1<<1))==0);	// 等待时钟设置OK  
 	TIM3->ARR=arr;  									// 设定计数器自动重装值 
	TIM3->PSC=psc;  									// 预分频器
	TIM3->SMCR=0;											// SMS[3:0]=0000,CK_INT作为时钟源
	TIM3->DIER|=1<<0;  								// 允许更新中断	  
	TIM3->CR1&=~(3<<5);								// CMS[1:0]=00,边沿对齐模式
	TIM3->CR1&=~(1<<4);								// DIR=0,向上计数模式
	TIM3->CR1|=0x01;									// 使能定时器3

  HAL_NVIC_EnableIRQ(TIM3_IRQn);
  HAL_NVIC_SetPriority(TIM3_IRQn, 4, 0);
}




void TIM4_Int_Init(uint16_t arr,uint16_t psc)
{
	RCC->APB1LENR|=1<<2;							//TIM4时钟使能  
	//如果开启TIM4时钟后,直接开始配置TIM4,则会有问题
	while((RCC->APB1LENR&(1<<2))==0);	//等待时钟设置OK  
 	TIM4->ARR=arr;  									//设定计数器自动重装值 
	TIM4->PSC=psc;  									//预分频器
	TIM4->SMCR=0;											//SMS[3:0]=0000,CK_INT作为时钟源
	TIM4->DIER|=1<<0;  								//允许更新中断	  
	TIM4->CR1&=~(3<<5);								//CMS[1:0]=00,边沿对齐模式
	TIM4->CR1&=~(1<<4);								//DIR=0,向上计数模式
	TIM4->CR1|=0x01;									//使能定时器4
  HAL_NVIC_EnableIRQ(TIM4_IRQn);
  HAL_NVIC_SetPriority(TIM4_IRQn, 5, 0);
}


void TIM5_Int_Init(uint32_t arr,uint32_t psc)
{
	RCC->APB1LENR|=1<<3;							  
	
	while((RCC->APB1LENR&(1<<3))==0);	  
 	TIM5->ARR=arr;  									//设定计数器自动重装值 
	TIM5->PSC=psc;  									//预分频器
	TIM5->SMCR=0;											//SMS[3:0]=0000,CK_INT作为时钟源
	TIM5->DIER|=1<<0;  								//允许更新中断	  
	TIM5->CR1&=~(3<<5);								//CMS[1:0]=00,边沿对齐模式
	TIM5->CR1&=~(1<<4);								//DIR=0,向上计数模式
	TIM5->CR1|=0x01;									//使能定时器5
  HAL_NVIC_EnableIRQ(TIM5_IRQn);
  HAL_NVIC_SetPriority(TIM5_IRQn, 6, 0);
}













