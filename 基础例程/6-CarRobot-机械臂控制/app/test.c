#include "contiki.h"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stimer.h>
#include "key.h"
#include "pcf8563.h"
#include "w25qxx.h"


PROCESS(test, "test");

void ble_write(char *buf);
void blood_stop(void);
void blood_start(void);
void sensor_init(void);
void sensor_poll(unsigned int t);
void config_init(void);

extern char phone[2][16];
float mlx90615_t();


PROCESS_THREAD(test, ev, data)
{
  static struct etimer _map_timer;
  PROCESS_BEGIN();


//  int id;
//  id = M25Pxx_Read_ID();
//  if (id != 0x00202015) {
//    printf("error can't find ic m25p16 \r\n");
//  } else {
//    printf("m25p16 id %08X\r\n", id);
//  }

//  config_init();

  sensor_init();

  etimer_set(&_map_timer, CLOCK_SECOND);
  while(1) {
    PROCESS_YIELD();
    if (ev == process_event_keydown) {
      printf("key down %d\r\n", (int)data);
      if ((int)data == KEY1) {
     //   blood_start();
      }
      if ((int)data == KEY2) {
     //   blood_stop();
      }

      if ((int)data == KEY3) {
       if (strlen(phone[0]) > 1) {
     //     gsm_request_call(phone[0]);
        }
      }

      if ((int)data == KEY4) {
        if (strlen(phone[1]) > 1) {
     //     gsm_request_call(phone[1]);
        }
      }
    }
    if (ev == process_event_keyup) {
      //printf("key up %d\r\n", (int)data);

    }
    if (etimer_expired(&_map_timer)) {
      /*
      printf("mlx90615_t %.1f\r\n", mlx90615_t());
      if (PCF8563_gettime(Time) < 0) {
        printf("rtc read error\r\n");
      }*/
      static unsigned int tick = 0;
      sensor_poll(++tick);
      etimer_set(&_map_timer, CLOCK_SECOND);
    }
  }
  PROCESS_END();
}