#include "tim3.h"
#include "led.h"
#include "stm32h743xx.h"
#include "stm32h7xx.h"
#include "stm32h7xx_hal_gpio.h"

#include "stm32h743xx.h"

void TIM3_Init(void) {
  // 1. 启用TIM3时钟（APB1总线）
  RCC->APB1LENR |= RCC_APB1LENR_TIM3EN;

  // 2. 配置预分频器（PSC）和自动重载值（ARR）
  TIM3->PSC = 0;      // 分频系数 = 0+1 = 1（时钟直接为240MHz）
  TIM3->ARR = 0xFFFF; // 任意值，此处不影响软件触发

  // 3. 使能更新事件中断
  TIM3->DIER |= TIM_DIER_UIE; // 允许更新中断

  // 4. 配置NVIC中断
  NVIC_SetPriority(TIM3_IRQn, 0);
  NVIC_EnableIRQ(TIM3_IRQn);

  // 注意：此处不启动计数器（CR1.CEN=0），完全通过软件触发
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