#include "motor.h"
#include "contiki.h"
#include "E18-D80NK.h"
#include "Ultrasonic.h"
#include "led.h"

/*********************************************************************************************
* 名称：tim4Pwm_init
* 功能：初始化定时器,用于PWM波(产生4路)
* 参数：arr:自动重装值
psc:自动预分频数
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void tim4Pwm_init(u16 arr,u16 psc)
{ 
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
    
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4 , ENABLE);       //使能定时器时钟 
	//使能GPIO外设时钟使能
 	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); 
	
    //输出TIM3 PWM脉冲波形
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;//TIM_CH1-4
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;      
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;              //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	        //速度100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;    
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource6,GPIO_AF_TIM4);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource7,GPIO_AF_TIM4);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource8,GPIO_AF_TIM4);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource9,GPIO_AF_TIM4);
    
	
	TIM_TimeBaseStructure.TIM_Period = arr; 
	TIM_TimeBaseStructure.TIM_Prescaler = psc; 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; 
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); 
	TIM_ARRPreloadConfig(TIM4, ENABLE);                         //使能TIMx在ARR上的预装载寄存器
    
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;   //输出极性
	TIM_OCInitStructure.TIM_Pulse = 0;                          //设置待装入捕获比较寄存器的脉冲值
	
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);//    
    TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);             //使能TIM4在CCR1上的预装载寄存器
    
	TIM_OCInitStructure.TIM_Pulse = 0; 
	TIM_OC2Init(TIM4, &TIM_OCInitStructure);//   
    TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);             //使能TIM4在CCR2上的预装载寄存器
    
	TIM_OCInitStructure.TIM_Pulse = 0; 
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);//    
    TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);             //使能TIM4在CCR3上的预装载寄存器
    
	TIM_OCInitStructure.TIM_Pulse = 0; 
	TIM_OC4Init(TIM4, &TIM_OCInitStructure);//     
    TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);             //使能TIM4在CCR4上的预装载寄存器
    
    
	TIM_CtrlPWMOutputs(TIM4,ENABLE);                            //MOE 主输出使能
	TIM_Cmd(TIM4, ENABLE);                                      //使能TIM3
}

/*********************************************************************************************
* 名称：motor_init
* 功能：初始化motor对应的GPIO
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void motor_init(void)
{
    //频率10KHz
    tim4Pwm_init(8399,0);
}


/*********************************************************************************************
* 名称：motor_control
* 功能：电机控制
* 参数：LeftPwm：左边电机PWM值
RightPwm：右边电机PWM值
Mode：运行模式
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void motor_control(u16 LeftPwm,u16 RightPwm,u8 Mode)
{
    if(LeftPwm>8400) LeftPwm=8400;
    if(RightPwm>8400) RightPwm=8400;
    
    //前
    if((Mode) == FRONT)                                     
    {       
        PWM1 = 0;
        PWM2 = LeftPwm;
        PWM3 = RightPwm;
        PWM4 = 0;	
    }
    //后
    else if((Mode) == REAR)                                
    {
        PWM1 = LeftPwm;
        PWM2 = 0;
        PWM3 = 0;
        PWM4 = RightPwm;
    }
    //左
    else if((Mode) == LEFT)                                
    {
        PWM1 = LeftPwm;
        PWM2 = 0;
        PWM3 = RightPwm;
        PWM4 = 0;
    }
    //右
    else if((Mode) == RIGHT)                               
    {
        PWM1 = 0;
        PWM2 = LeftPwm;
        PWM3 = 0;
        PWM4 = RightPwm;
    }
    else
    {
        PWM1 = 0;
        PWM2 = 0;
        PWM3 = 0;
        PWM4 = 0;
    }
}


/*********************************************************************************************
移动模式参数
*********************************************************************************************/
/*
0：停止
1：手动
2：寻迹
3：避障
*/
u8 motion_mode=0;

/*********************************************************************************************
电机控制参数
*********************************************************************************************/
u8 motor_direction=0;
u16 motor_speed=0;

//申明进程
PROCESS(motor,"motor");

/*********************************************************************************************
* 名称: PROCESS_THREAD()
* 功能: 定义进程
* 参数:
* 返回: 无
* 修改:
* 注释:
*********************************************************************************************/
PROCESS_THREAD(motor, ev, data)
{
    static struct etimer motor1_timer;
    
    PROCESS_BEGIN();
    
    motor_init();                                               //电机初始化
    E18D80NK_init();                                            //红外寻迹初始化
    etimer_set(&motor1_timer,20);                               //设置etimer定时器
    
    while (1) 
    {        
        PROCESS_WAIT_EVENT();
        
        if(ev == PROCESS_EVENT_TIMER)
        {
            etimer_reset(&motor1_timer); 
            
            /*手动控制模式*/
            if(motion_mode==1)
            {
                motor_control(motor_speed,motor_speed,motor_direction);
            }
            /*寻迹模式*/
            else if(motion_mode==2)
            {
                u8 infraredSta = get_state();
                if((infraredSta&0x03)==0x03)
                {
                    led3 = 0;
                    led4 = 0;
                    motor_control(motor_speed,motor_speed,STOP);//停止                  
                }
                else
                {
                    if((infraredSta&0x01)==0x01)
                    { 
                        led3 = 1;
                        led4 = 0;
                        motor_control(motor_speed,motor_speed,LEFT);//左转
                    }
                    else if((infraredSta&0x02)==0x02)
                    {      
                        led3 = 0;
                        led4 = 1;
                        motor_control(motor_speed,motor_speed,RIGHT);//右转
                    }
                    else
                    {        
                        led3 = 1;
                        led4 = 1;
                        motor_control(motor_speed,motor_speed,FRONT);//前进
                    }
                }
            }
            /*避障模式*/
            else if(motion_mode==3)
            {
                if(ultrasonicValue<30)
                {                    
                    motor_control(0,0,0);
                }
                else
                {                    
                    motor_control(motor_speed,motor_speed,FRONT);
                }
            }
            else
            {               
                motor_control(0,0,0);
            }
            
            if(motion_mode!=2)
            {      
                led3 = 1;
                led4 = 1; 
            }
        }
    }
    
    PROCESS_END();
}

