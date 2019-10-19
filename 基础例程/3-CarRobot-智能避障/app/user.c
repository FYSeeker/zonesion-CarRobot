#include "user.h"
#include "rtc.h"
#include "motor.h"
#include "led.h"
#include "adc_key.h"
#include "sample.h"
#include "user.h"


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
    etimer_set(&user_timer,100);                                //设置etimer定时器
    
    while(1)
    {
        PROCESS_WAIT_EVENT();  
        
        if(ev == process_event_key)                             //判断是否发生按键事件
        {                               
            if((*(int*)data)==KEY1_PRESSED)                     //判断事件传入的值
            {
                motor_run = !motor_run;
            }   
            else if((*(int*)data)==KEY3_PRESSED)                //判断事件传入的值
            {
                if(motor_speed<8000)
                    motor_speed += 1000;
                    
            }
            else if((*(int*)data)==KEY4_PRESSED)                //判断事件传入的值
            {
                if(motor_speed>5000)
                    motor_speed -= 1000;
            }
        }
        
        if(ev == PROCESS_EVENT_TIMER)
        {
            etimer_reset(&user_timer); 
            
            sensor_poll();              
        }
    }
    
    PROCESS_END();
}


