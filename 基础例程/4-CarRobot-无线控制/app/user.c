#include "user.h"
#include "zxbee.h"
#include "rtc.h"
#include "config.h"
#include "motor.h"
#include "SteeringEngine.h"
#include "led.h"
#include "adc_key.h"
#include "config.h"
#include "mg2639.h"
#include "sample.h"
#include "ble.h"


//LED控制参数：0-->关灯，1-->开灯
uint8 D1=0;


/*********************************************************************************************
网络参数
*********************************************************************************************/
//连接模式：0-->蓝牙，1-->gsm
uint8 connect_mode=0;
//无线MAC
char RADIO_MAC[32];


extern char qrcodeShow_flag;


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
        
        if(ev == process_event_key) //判断是否发生按键事件
        {                               
            if((*(int*)data)==KEY1_PRESSED)//事件传入的值
            {
                if(qrcodeShow_flag==0)
                {
                    connect_mode = !connect_mode;
                    
                    if(connect_mode==0)                             //蓝牙连接模式
                    {                
                        ble_init();
                        config_save_mode();
                    }
                    else                                            //GSM连接模式
                    {
                        mg2639_init();
                        config_save_mode();
                    }
                }
            }            
        }
        
        if(ev == PROCESS_EVENT_TIMER)
        {
            etimer_reset(&user_timer); 
            
            sensor_poll();  
            led2 = !D1;                                         //更新LED状态
            led3 = !D1;                                         //更新LED状态
            led4 = !D1;                                         //更新LED状态
        }
    }
    
    PROCESS_END();
}

            
          