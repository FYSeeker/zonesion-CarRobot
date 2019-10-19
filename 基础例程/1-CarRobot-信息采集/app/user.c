#include "user.h"
#include "rtc.h"
#include "led.h"
#include "sample.h"


//申明进程
PROCESS(user,"user");

/*********************************************************************************************
* 名称: PROCESS_THREAD()
* 功能: 定义进程
* 参数:
* 返回: 无
* 修改:
* 注释:
*********************************************************************************************/
PROCESS_THREAD(user, ev, data)
{    
    static struct etimer user_timer;
    
    PROCESS_BEGIN();
    
    sensor_init();                                              //传感器初始化
    etimer_set(&user_timer,200);                                //设置etimer定时器
    
    while(1)
    {  
        sensor_poll();                 //传感器轮询函数
        
        PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);    //等待etimer定时器溢出事件
        etimer_reset(&user_timer);                              //设置etimer定时器
    }
    
    PROCESS_END();
}

            
          