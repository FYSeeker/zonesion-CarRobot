#ifndef _SteeringEngine_H_
#define _SteeringEngine_H_

#include "stm32f4xx.h"
#include "extend.h"
#include "contiki.h"

#define STEER1 TIM3->CCR1		//CH1
#define STEER2 TIM3->CCR2		//CH2
#define STEER3 TIM3->CCR3		//CH3
#define STEER4 TIM3->CCR4		//CH4


/*********************************************************************************************
* 名称：steeringEngine_init
* 功能：舵机初始化
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void steeringEngine_init();

#endif