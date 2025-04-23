#ifndef __TIMER_H
#define __TIMER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
extern int32_t timer5counter;

void TIM3_Int_Init(uint16_t arr,uint16_t psc); 
void TIM4_Int_Init(uint16_t arr,uint16_t psc);
void TIM5_Int_Init(uint32_t arr,uint32_t psc);

int32_t getTim5Counter();

#ifdef __cplusplus
}
#endif

#endif























