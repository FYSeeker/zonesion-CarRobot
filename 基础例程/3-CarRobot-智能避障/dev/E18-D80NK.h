
#ifndef D80NK
#define D80NK

#include "stm32f4xx.h"


#define         pin_left        GPIO_Pin_12
#define         left_port       GPIOB

#define         pin_right       GPIO_Pin_13
#define         right_port      GPIOB

#define         RCC_D80NK       RCC_AHB1Periph_GPIOB


extern void E18D80NK_init(void);
extern u8 get_state(void);

#endif