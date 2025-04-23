#include "main.h"

void TIM3_Init(void) {
  // 1. 启用TIM3时钟（APB1总线）
  RCC->APB1LENR |= RCC_APB1LENR_TIM3EN;

  // 2. 配置预分频器（PSC）和自动重载值（ARR）
  TIM3->PSC = 240;      // 分频系数 = 0+1 = 1（时钟直接为240MHz）
  TIM3->ARR = 60000; // 任意值，此处不影响软件触发

  
}

/**
 * @brief
 * 此中断处理程序只是为了验证程序运行过程中，经过我修改SCB->VTOR后，还能不能找到我的中断向量表
 */
void TIM3_IRQHandler(void) {
  if (TIM3->SR & TIM_SR_UIF) {
    LED1_Toggle;
    TIM3->SR &= ~TIM_SR_UIF; // 清除 UIF 标志
  }
}