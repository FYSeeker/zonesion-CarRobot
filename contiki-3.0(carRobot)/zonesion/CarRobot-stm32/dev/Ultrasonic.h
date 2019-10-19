#ifndef Ultrasonic
#define Ultrasonic

#include "stm32f4xx.h"
#include "stm32f4xx_exti.h"


#define RCC_trig                        RCC_AHB1Periph_GPIOB
#define trig_pin                        GPIO_Pin_14             //pb14
#define trig_port                       GPIOB                   

#define RCC_echo                        RCC_AHB1Periph_GPIOB
#define echo_pin                        GPIO_Pin_15              //pb15
#define echo_port                       GPIOB


/*定义全局变量ultrasonicValue，单位cm*/
extern float ultrasonicValue;

#endif