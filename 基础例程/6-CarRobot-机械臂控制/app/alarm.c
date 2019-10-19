#include "alarm.h"
#include "pcf8563.h"
#include "rtc.h"
#include "delay.h"
#include "pic-alarm.h"
#include "ili93xx.h"
#include "sample.h"

PROCESS(alarm, "alarm");

char alarms[2][3];
extern char V6[32];


PROCESS_THREAD(alarm, ev, data)
{ 
    static unsigned int tick = 0;
    static char wday = 0, hour = 0, mini = 0, sec = 0;
    static struct etimer et;
    
    PROCESS_BEGIN(); 
    //vibrator_init();
    
    RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct);
    RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);
    hour = RTC_TimeStruct.RTC_Hours;
    mini = RTC_TimeStruct.RTC_Minutes;
    sec = RTC_TimeStruct.RTC_Seconds;
    wday = RTC_DateStruct.RTC_WeekDay;
    
    while (1) {
        etimer_set(&et, CLOCK_SECOND);
        PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
        tick ++;
        if (tick == 30) {    
            tick = 0;
            RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct);
            RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);
            hour = RTC_TimeStruct.RTC_Hours;
            mini = RTC_TimeStruct.RTC_Minutes;
            sec = RTC_TimeStruct.RTC_Seconds;
            wday = RTC_DateStruct.RTC_WeekDay;
        } else {
            sec ++;
            if (sec == 60) {
                sec = 0;
                mini ++;
                if (mini == 60) {
                    mini = 0;
                    hour ++;
                    if (hour == 24) {
                        hour = 0;
                        wday ++;
                        if (wday == 7) {
                            wday = 0;
                        }
                    }
                }
            }
        }
        char a = 0;
        if (alarms[0][0] != 0 && hour==alarms[0][1] && mini==alarms[0][2]) a = 1;
        if (alarms[1][0] != 0 && hour==alarms[1][1] && mini==alarms[1][2]) a = 1;
        if (a != 0) {
            if (tick%2 == 0) {
                LCD_ShowPic(280,20,30,30,pic_alarm);
            }
            else {
                LCD_Clear(280,20,280+30,50,0x4596);
            }
        }
        else {
            LCD_Clear(280,20,280+30,50,0x4596);
        }
    }
    
    PROCESS_END();
}