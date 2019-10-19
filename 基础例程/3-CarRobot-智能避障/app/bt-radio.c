
#include <contiki.h>
#include "usart.h"

PROCESS(bt_radio, "bt-radio");


static int _recv_ch(char c)
{
#define BUF_SIZE 64
  
  static char _bufs[2][BUF_SIZE];
  static char offset = 0;
  static char *buf = _bufs[0];
  
  printf("%c", c);
  if (offset == 0 && c == '{') {
    buf[offset++] = c;
  } else if (offset > 0 && offset < BUF_SIZE-1) {
    buf[offset++] = c;
    if (c == '}') {
      buf[offset] = 0;
      zxbee_on_recv(buf);
      offset = 0;
      if (buf == _bufs[0]) buf = _bufs[1];
      else buf = _bufs[0];
    }
  } else {
    if (offset == BUF_SIZE-1) {
      offset = 0;
    }
  }
  return 0;
}
void zigbee_send(char *str)
{
  int len = strlen(str);
  for (int i=0; i<len; i++) {
    uart6_putc(str[i]);
  }
}

PROCESS_THREAD(bt_radio, ev, data)
{
  PROCESS_BEGIN();
  
  uart6_init(115200);
  uart6_set_input(_recv_ch);
  
  
  PROCESS_END();
}