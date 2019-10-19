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

/*********************************************************************************************
小车控制参数
*********************************************************************************************/
/*
小车移动模式
D1：0x00-->停止，0x01-->手动模式，0x02-->寻迹模式，0x04-->避障模式
*/
uint8 D1=0;
/*
手动模式电机控制参数
V2：电机速度（0--84）
D2：运动方向（0x00-->停止，0x01-->前进，0x02-->后退，0x04-->左转，0x08-->右转）
*/
uint8 V2=0,D2=0;

/*********************************************************************************************
机械臂控制参数
*********************************************************************************************/
uint8 V3=6,V4=10,V5=90,V6=0;

/*
机械臂自定义动作控制--V7
0：无动作
1：自检
2：倒水
3：捡垃圾
4：扔东西
*/
uint8 V7=0;


/*********************************************************************************************
网络参数
*********************************************************************************************/
//连接模式：0-->蓝牙，1-->gsm
uint8 connect_mode=0;
//无线MAC
char RADIO_MAC[32];

process_event_t d2Recv_event;


void config_save_alarm();
void config_save_phone();
void config_save_notify();


extern void uartData_handle();
extern void ble_init(void);

extern char qrcode_flag;


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
    static u16 user_t=0,d2_t=0;
    
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
                if(qrcode_flag==0)
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
        
        if(ev == d2Recv_event)
        {
            d2_t=0;
        }
        
        if(ev == PROCESS_EVENT_TIMER)
        {
            etimer_reset(&user_timer); 
            
//            uartData_handle();

            /*更新移动模式参数*/           
            motion_mode= D1;
            
            /*更新电机参数*/
            motor_direction = D2;
            motor_speed = V2*100;
            
            /*更新机械臂参数*/
            robotArm.x = V3;
            robotArm.y = V4;
            robotArm.angle_3 = V5;
            robotArm.angle_4 = V6;            
            
            /*判断V7，运行机械臂自定义动作*/
            if(V7!=0)
            {                
                process_post(&robotArm_motion,robotArm_motion_event,&V7);//发送机械臂动作事件
            }
            
            user_t++;
            d2_t++;
            
            if((user_t%5)==0)
            {
                user_t = 0;
                sensor_poll();                 
            }                                             //传感器轮询函数
            
            /*2s后复位,防失联*/
            if(d2_t>19)
            {
                d2_t = 0;
//                D2 = 0;
            }
        }
    }
    
    PROCESS_END();
}

            
          