#ifndef _sample_h_
#define _sample_h_

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "stm32f4xx_rtc.h"
#include "contiki.h"

extern RTC_TimeTypeDef RTC_TimeStruct;                          //时间
extern RTC_DateTypeDef RTC_DateStruct;                          //日期
extern float A0, A1, A2;                                        //光照度、温度、湿度

void sensor_init(void);
void sensor_poll(void);

#endif