#include "main.h"
			
extern int mainLoopFlg;
int32_t timer5counter;
//extern int32_t timeNow;


//��ʱ��3�жϷ������	 		//TIM3 serves the main loop
// void TIM3_IRQHandler(void)
// { 		  
// 	static uint8_t led1sta=1;
// 	if(TIM3->SR&0X0001)		//����ж�
// 	{
// //		LED1(led1sta^=1);	 
// 		mainLoopFlg = 1;
// 	}				   
// 	TIM3->SR&=~(1<<0);		//����жϱ�־λ 	    
// }


//��ʱ��4�жϷ������	 
void TIM4_IRQHandler(void)
{ 		  
	static uint8_t led1sta=1;
	if(TIM4->SR&0X0001)		//����ж�
	{
//		LED1(led1sta^=1);	 
	}				   
	TIM4->SR&=~(1<<0);		//����жϱ�־λ 	    
}

//��ʱ��5�жϷ������	 
void TIM5_IRQHandler(void)
{ 		  
	static uint8_t led1sta=1;
	if(TIM5->SR&0X0001)		//����ж�
	{
			timer5counter++;
	}				
//	int32_t temp_timeNow = 0;
//	temp_timeNow = TIM5->CNT;
//	timeNow = temp_timeNow;
	TIM5->SR&=~(1<<0);		//����жϱ�־λ 	    
}

int32_t getTim5Counter()
{
	uint64_t Now = 0;
	Now = TIM5->CNT;
	return Now;
}

//ͨ�ö�ʱ��3�жϳ�ʼ��
//TIM3��ʱ������APB1,��D2PPRE1��2��Ƶ��ʱ��
//TIM3��ʱ��ΪAPB1ʱ�ӵ�2������APB1Ϊ100M
//����,TIM3��ʱ��Ƶ��Ϊ200Mhz
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=��ʱ������Ƶ��,��λ:Mhz
//����ʹ�õ��Ƕ�ʱ��3!
void TIM3_Int_Init(uint16_t arr,uint16_t psc)
{
	RCC->APB1LENR|=1<<1;							//TIM3ʱ��ʹ�� 		�ֲ�P438 
	//�������TIM3ʱ�Ӻ�,ֱ�ӿ�ʼ����TIM3,���������
	while((RCC->APB1LENR&(1<<1))==0);	// �ȴ�ʱ������OK  
 	TIM3->ARR=arr;  									// �趨�������Զ���װֵ 
	TIM3->PSC=psc;  									// Ԥ��Ƶ��
	TIM3->SMCR=0;											// SMS[3:0]=0000,CK_INT��Ϊʱ��Դ
	TIM3->DIER|=1<<0;  								// ��������ж�	  
	TIM3->CR1&=~(3<<5);								// CMS[1:0]=00,���ض���ģʽ
	TIM3->CR1&=~(1<<4);								// DIR=0,���ϼ���ģʽ
	TIM3->CR1|=0x01;									// ʹ�ܶ�ʱ��3

  HAL_NVIC_EnableIRQ(TIM3_IRQn);
  HAL_NVIC_SetPriority(TIM3_IRQn, 4, 0);
}




void TIM4_Int_Init(uint16_t arr,uint16_t psc)
{
	RCC->APB1LENR|=1<<2;							//TIM4ʱ��ʹ��  
	//�������TIM4ʱ�Ӻ�,ֱ�ӿ�ʼ����TIM4,���������
	while((RCC->APB1LENR&(1<<2))==0);	//�ȴ�ʱ������OK  
 	TIM4->ARR=arr;  									//�趨�������Զ���װֵ 
	TIM4->PSC=psc;  									//Ԥ��Ƶ��
	TIM4->SMCR=0;											//SMS[3:0]=0000,CK_INT��Ϊʱ��Դ
	TIM4->DIER|=1<<0;  								//��������ж�	  
	TIM4->CR1&=~(3<<5);								//CMS[1:0]=00,���ض���ģʽ
	TIM4->CR1&=~(1<<4);								//DIR=0,���ϼ���ģʽ
	TIM4->CR1|=0x01;									//ʹ�ܶ�ʱ��4
  HAL_NVIC_EnableIRQ(TIM4_IRQn);
  HAL_NVIC_SetPriority(TIM4_IRQn, 5, 0);
}


void TIM5_Int_Init(uint32_t arr,uint32_t psc)
{
	RCC->APB1LENR|=1<<3;							  
	
	while((RCC->APB1LENR&(1<<3))==0);	  
 	TIM5->ARR=arr;  									//�趨�������Զ���װֵ 
	TIM5->PSC=psc;  									//Ԥ��Ƶ��
	TIM5->SMCR=0;											//SMS[3:0]=0000,CK_INT��Ϊʱ��Դ
	TIM5->DIER|=1<<0;  								//��������ж�	  
	TIM5->CR1&=~(3<<5);								//CMS[1:0]=00,���ض���ģʽ
	TIM5->CR1&=~(1<<4);								//DIR=0,���ϼ���ģʽ
	TIM5->CR1|=0x01;									//ʹ�ܶ�ʱ��5
  HAL_NVIC_EnableIRQ(TIM5_IRQn);
  HAL_NVIC_SetPriority(TIM5_IRQn, 6, 0);
}













