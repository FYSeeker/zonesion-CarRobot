#ifndef _user_h_
#define _user_h_

#include <stdio.h>
#include "stdlib.h"
#include "string.h"
#include "contiki.h"

extern uint8 V2,D2;
extern uint8 V3,V4,V5,V6;
extern uint8 V7;

extern uint8 connect_mode;
extern char RADIO_MAC[32];

//ÉùÃ÷½ø³Ì
extern struct process user;

extern process_event_t d2Recv_event;

#endif