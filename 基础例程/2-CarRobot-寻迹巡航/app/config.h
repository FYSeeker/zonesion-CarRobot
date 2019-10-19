#ifndef _CONFIG_H_
#define _CONFIG_H_
#include "stm32f4xx.h"

void config_save_alarm();
void config_read_alarm();
void config_save_mode();
void config_read_mode();
void config_init(void);

#endif