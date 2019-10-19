
#include <contiki.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "stm32f4xx.h"
#include "string.h"
PROCESS(fallalg, "fall");

extern gsm_info_t gsm_info;
extern char notify;

int fall_dect(float x, float y, float z)
{
  float x2 = x * x;
  float y2 = y * y;
  float z2 = z * z;
  float a;

  static unsigned int tm1 = 0, tm2 = 0, tm3 = 0, tm4 = 0;

  a = sqrt(x2 + y2 + z2);
  if (a < 5 && tm1 == 0) {
    tm1 = clock_time();
    //printf("xxx tm1 %d, a %.1f\r\n", tm1, a);
  } else if (tm1 != 0) {
    //printf(" a %.1f \r\n", a);
    if (a > 5) tm2 = clock_time();
    if (a > 40) tm3 = clock_time();
    float d = a - 16;
    if (abs((int)d) < 2) {
      tm4 = clock_time();
      printf("d %.1f\r\n", d);
    }
    //if (abs(a-16) > 3) tm5 = clock_time();
  }
  if (tm1!=0 && tm2!=0 && tm3 !=0 && tm4!=0) {
    /*
     * tm1 ~ tm2  30ms--400ms tm2 ~ tm3 < 50ms
     */
    int t1 = (int)tm2  - (int) tm1;
    int t2 = (int)tm3 - (int) tm2;

    printf("%d,%d,%d,%d\r\n", tm1, tm2, tm3, tm4);

    if (t1 > 30 && t1 < 400 && t2 < 50) {
     tm1 = tm2 = tm3 = tm4 = 0;
     return 1;
    }
    tm1 = tm2 = tm3 = tm4 = 0;
  }
  if (tm1 != 0 && ((int)clock_time() - (int)(tm1 + 1000)) > 0) {
    //printf("%d,%d,%d,%d\r\n", tm1, tm2, tm3, tm4);
    tm1 = tm2 = tm3 = tm4 = 0;
  }
  return 0;
}

PROCESS_THREAD(fallalg, ev, data)
{
  static struct etimer et;

  PROCESS_BEGIN();

  LIS3DH_init();
  etimer_set(&et, CLOCK_SECOND/100); // 10 ms
  while (1) {
    PROCESS_YIELD();
    if (etimer_expired(&et)) {
      u8_t st;
      if (1 == LIS3DH_GetStatusReg(&st)) {
        if (st & 0x04) {
          AxesRaw_t acc;
          if (1 == LIS3DH_GetAccAxesRaw(&acc)) {
            float x, y, z;
            x = acc.AXIS_X*0.004f;
            y = acc.AXIS_Y*0.004f;
            z = acc.AXIS_Z*0.004f;
            //printf("{%.1f,%.1f,%.1f}\r\n", x, y, z);
            if (fall_dect(x, y, z) == 1) {
              printf("check fall!\r\n");

              if (gsm_info.simcard_status == 1 && notify != 0) {
                if (gsm_info.phone_status == PHONE_ST_IDLE) {
                  extern char phone[2][16];
                  char *p = phone[0];
                  if (strlen(p) > 1)
                      gsm_request_call(p);    //²¦Í¨µç»°1
                  else
                      printf("contact 0 not set\r\n");
                }
              }
            }
          }
        }
      }
      etimer_set(&et, CLOCK_SECOND/100); // 10 ms
    }
  }
  PROCESS_END();
}