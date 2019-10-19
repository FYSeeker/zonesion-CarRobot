#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "usart.h"
#include "stm32f4xx.h"


#define ZIGBEE_CHANNEL  11


void uart6_set_input(int(*fun)(unsigned char));
PROCESS(zigbee_uart_process, "zigbee uart driver");

static process_event_t uart_command_event;

static char z_mac[24];
static unsigned int z_panid;
static unsigned int z_channel;
static void (*_onrecv_call)(char *, int) = NULL;
static void _zigbee_send(char *buf, int len);
static int z_type = -1;

char* zigbee_mac() 
{
  return z_mac;
}
unsigned int zigbee_panid()
{
  return z_panid;
}
unsigned int zigbee_channel()
{
  return z_channel;
}
 int zigbee_type()
{
  return z_type;
}

void zigbee_send(char *str)
{
  char buf[32];
  int r;
  int len = strlen(str);
  r = sprintf(buf, "at+send=%u\n", len);
  _zigbee_send(buf, r);
  _zigbee_send(str, len);
}
void zigbee_set_onrecv(void (*f)(char *, int))
{
  _onrecv_call = f;
}
static void _zigbee_send(char *buf, int len)
{
  for (int i=0; i<len; i++) {
    USART_SendData(USART6,*buf++);
  }
}


static int uart_rx_offset = 0;
static int _zigbee_input_byte(unsigned char c)
{
#define recv_timeout 10
  
  #define buf_size 96  
  static char uart_buf[2][buf_size];
  #define buf1 uart_buf[0]
  #define buf2 uart_buf[1]
  static char* pbuf = buf1;

  static unsigned int ltm = 0;
  unsigned int tm = clock_time()*(1000/CLOCK_SECOND);
  int dt = (int)tm  - (int)(ltm+recv_timeout);
  
  static int data = 0;
  ltm = tm;
  if (dt > 0) {
    uart_rx_offset = 0;
    data = 0;
  }
  if (data == 0) {
    if (c == '\n') {
      if (uart_rx_offset > 0) {
        pbuf[uart_rx_offset] = 0;
        if (uart_rx_offset>6 && memcmp(pbuf, "+recv=", 6) == 0) {
          data = atoi(&pbuf[6]);
          uart_rx_offset++;
        } else {
          process_post(&zigbee_uart_process, uart_command_event, pbuf);
          if (pbuf == buf1) pbuf = buf2;
          else pbuf = buf1;
          uart_rx_offset = 0;
        }
      } 
    } else {
      if (uart_rx_offset>=0 && uart_rx_offset<buf_size) {
        pbuf[uart_rx_offset++] = c;
      } else uart_rx_offset = -1;
    }
  } else if (data > 0) {
    if (uart_rx_offset>=0 && uart_rx_offset<buf_size) {
      pbuf[uart_rx_offset++] = c;
    } else uart_rx_offset = -1;
    data --;
    if (data == 0) {
      process_post(&zigbee_uart_process, uart_command_event, pbuf);
      if (pbuf == buf1) pbuf = buf2;
      else pbuf = buf1;
      uart_rx_offset = 0;
    }
  }
  
  return 0;
}


PROCESS_THREAD(zigbee_uart_process, ev, data)
{
  static struct etimer _timer;
  static int init_stetp = 0;
  static char* cmd[] = {"at\n", "at+mac=?\n", "at+panid=?\n", "at+type=?\n",  "at+channel=?\n"};
  
  PROCESS_BEGIN();
  
  usart6_init(115200);
  uart6_set_input(_zigbee_input_byte);
  uart_command_event = process_alloc_event();
  
  etimer_set(&_timer, 500*CLOCK_SECOND/1000);
  while (1) {
    PROCESS_YIELD();
    if ((init_stetp < sizeof cmd /sizeof cmd[0]) && etimer_expired(&_timer)) {
        _zigbee_send_str(cmd[init_stetp]);
        etimer_set(&_timer, 200*CLOCK_SECOND/1000);
        init_stetp ++;
    } 
    if (ev == uart_command_event) {
        printf("<<< %s\r\n", (char*)data);
    }
  }
  
  PROCESS_END();
}