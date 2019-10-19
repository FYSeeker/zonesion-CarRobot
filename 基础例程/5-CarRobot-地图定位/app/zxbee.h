#ifndef __ZXBEE_H__
#define __ZXBEE_H__

#include "contiki.h"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void zxbeeBegin(void);
int zxbeeAdd(char* tag, char* val);
char* zxbeeEnd(void);
int z_process_command_call(char *tag, char *val);
void zxbee_onrecv_fun(char *pkg, int len);

#endif