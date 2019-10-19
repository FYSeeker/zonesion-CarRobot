#ifndef _INFRARED_H_
#define _INFRARED_H_

#include "stm32f4xx.h"
#include "stdio.h"

void  infrared_init(void);
int infrared_get(void);

#endif