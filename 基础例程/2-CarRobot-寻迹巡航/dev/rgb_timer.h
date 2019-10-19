#ifndef _RGB_TIMER_H_
#define _RGB_TIMER_H_
#include "stm32f4xx.h"

void TIM3_Init(unsigned short period, unsigned short prescaler);
void TIM4_Init(unsigned short period, unsigned short prescaler);
void rgb_init(void);

#endif