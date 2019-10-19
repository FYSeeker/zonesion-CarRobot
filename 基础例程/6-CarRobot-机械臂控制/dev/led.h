#ifndef _LED_H_
#define _LED_H_

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "stm32f4xx.h"
#include "extend.h"
#include "contiki.h"

#define led1    PEout(2)
#define led2    PEout(3)
#define led3    PAout(5)
#define led4    PAout(7)

#define LED1    0X01
#define LED2    0X02
#define LED3    0X04
#define LED4    0X08
#define LEDR    0X10
#define LEDG    0X20
#define LEDB    0X40

void led_init(void);
void turn_on_off(unsigned char Led);
unsigned char get_led_status(void);


#endif

