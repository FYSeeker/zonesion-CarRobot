#include "SteeringEngine.h"
#include "led.h"
#include "delay.h"
#include "adc_key.h"
#include "stdio.h"
#include "math.h"
#include "adc_key.h"
#include "user.h"


/*1号和2号舵机pwm比价值*/
uint16 steer1Pwm=2500,steer2Pwm=500;

/*********************************************************************************************
* 名称：tim2Pwm_init
* 功能：初始化定时器2,用于PWM波(产生2路)
* 参数：arr:自动重装值
        psc:自动预分频数
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void tim2Pwm_init(u16 arr,u16 psc)
{ 
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);       //使能定时器时钟 
	//使能GPIO外设时钟使能
 	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); 
	
    //输出TIM2 PWM脉冲波形
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3;//TIM_CH3-4
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;      
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;              //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	        //速度100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;    
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_TIM2);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_TIM2);
	
	TIM_TimeBaseStructure.TIM_Period = arr; 
	TIM_TimeBaseStructure.TIM_Prescaler = psc; 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; 
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); 
	TIM_ARRPreloadConfig(TIM2, ENABLE);                         //使能TIMx在ARR上的预装载寄存器


	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//比较输出使能
	TIM_OCInitStructure.TIM_Pulse = 0;                          //设置待装入捕获比较寄存器的脉冲值
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;   //输出极性
	
	TIM_OC3Init(TIM2, &TIM_OCInitStructure);//    
	TIM_OCInitStructure.TIM_Pulse = 0; 
	TIM_OC4Init(TIM2, &TIM_OCInitStructure);//     
	
	TIM_Cmd(TIM2, ENABLE);                                      //使能TIM2
	TIM_CtrlPWMOutputs(TIM2,ENABLE);                            //MOE 主输出使能
}


/*********************************************************************************************
* 名称：tim3Pwm_init
* 功能：初始化定时器,用于PWM波(产生4路)
* 参数：arr:自动重装值
        psc:自动预分频数
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void tim3Pwm_init(u16 arr,u16 psc)
{ 
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , ENABLE);       //使能定时器时钟 
	//使能GPIO外设时钟使能
 	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); 
	
    //输出TIM3 PWM脉冲波形
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;//TIM_CH1-4
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;      
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;              //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	        //速度100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;    
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_TIM3);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource7,GPIO_AF_TIM3);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource8,GPIO_AF_TIM3);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource9,GPIO_AF_TIM3);

	
	TIM_TimeBaseStructure.TIM_Period = arr; 
	TIM_TimeBaseStructure.TIM_Prescaler = psc; 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; 
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); 
	TIM_ARRPreloadConfig(TIM3, ENABLE);                         //使能TIMx在ARR上的预装载寄存器

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;   //输出极性
	TIM_OCInitStructure.TIM_Pulse = 2500;                          //设置待装入捕获比较寄存器的脉冲值
	
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);// 
    TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);             //使能TIM4在CCR1上的预装载寄存器   
    
	TIM_OCInitStructure.TIM_Pulse = 500; 
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);//  
    TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);             //使能TIM4在CCR2上的预装载寄存器  
    
	TIM_OCInitStructure.TIM_Pulse = 1500; 
	TIM_OC3Init(TIM3, &TIM_OCInitStructure);//    
    TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);             //使能TIM4在CCR3上的预装载寄存器  
    
	TIM_OCInitStructure.TIM_Pulse = 2270; 
	TIM_OC4Init(TIM3, &TIM_OCInitStructure);//     
    TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);             //使能TIM4在CCR4上的预装载寄存器  
    	
	TIM_CtrlPWMOutputs(TIM3,ENABLE);                            //MOE 主输出使能
	TIM_Cmd(TIM3, ENABLE);                                      //使能TIM3
}



/*********************************************************************************************
* 名称: tim5Int_init
* 功能: 定时器中断初始化
* 参数: 
* 返回: 无
* 修改:
* 注释:
*********************************************************************************************/
void tim5Int_init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);         //使能TIM时钟
    
    TIM_TimeBaseInitStructure.TIM_Period = arr; 	            //自动重装载值
    TIM_TimeBaseInitStructure.TIM_Prescaler= psc;               //定时器分频
    TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;//向上计数模式
    TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
    
    TIM_TimeBaseInit(TIM5,&TIM_TimeBaseInitStructure);          //初始化TIM
    
      TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE);                  //允许定时器更新中断

    NVIC_InitStructure.NVIC_IRQChannel=TIM5_IRQn;               //定时器中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;     //抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;            //子优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    TIM_Cmd(TIM5,ENABLE);   
}

//定时器5中断服务函数,用于控制舵机
void TIM5_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM5,TIM_IT_Update)==SET) //溢出中断
    {
        if(STEER1<steer1Pwm) STEER1++;
        else if(STEER1>steer1Pwm) STEER1--;
        
        if(STEER2<steer2Pwm) STEER2++;
        else if(STEER1>steer2Pwm) STEER2--;
    }
    TIM_ClearITPendingBit(TIM5,TIM_IT_Update);  //清除中断标志位
}




/*********************************************************************************************
* 名称：steeringEngine_init
* 功能：舵机初始化
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void steeringEngine_init()
{
    //频率200Hz
    tim3Pwm_init(5000,84);
    //2ms中断
    tim5Int_init(2000,84);
    
    steer1Pwm = 2000;
    steer2Pwm = 500;
    STEER3 = 1500;
    STEER4 = 2270;
}




/*********************************************************************************************
* 名称：steer1_control
* 功能：舵机控制
* 参数：angle:舵机转动角度，0-160
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void steer1_control(float angle)
{
    angle = 180-angle;
    if(angle<20) angle = 20;
    if(angle>180) angle = 180;  
    
    steer1Pwm = 500+(u16)(angle*11.11);
}


/*********************************************************************************************
* 名称：steer2_control
* 功能：舵机控制
* 参数：angle:舵机转动角度，0-180,
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void steer2_control(float angle)
{
    if(angle<0) angle = 0;
    if(angle>180) angle = 180;  
    
    steer2Pwm = 500+(u16)(angle*11.11);
}


/*********************************************************************************************
* 名称：steer3_control
* 功能：舵机控制
* 参数：angle:舵机转动角度，0-180
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void steer3_control(float angle)
{
    if(angle<0) angle = 0;
    if(angle>180) angle = 180;
    
    STEER3 = 500+(u16)(angle*11.11);
}


/*********************************************************************************************
* 名称：steer4_control
* 功能：舵机控制
* 参数：angle:舵机转动角度，0-70
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void steer4_control(float angle)
{
    angle = 160-angle;
    if(angle<90) angle = 90;
    if(angle>160) angle = 160;
    
    STEER4 = 500+(u16)(angle*11.11);
}



/*声明进程*/
PROCESS(robot_arm,"robot_arm");

/*********************************************************************************************
* 名称: PROCESS_THREAD()
* 功能: 定义进程：机械臂控制进程
* 参数: ev:事件，data:事件所带的数据
* 返回: 无
* 修改:
* 注释:
*********************************************************************************************/
PROCESS_THREAD(robot_arm, ev, data)
{ 
    static struct etimer robotArm_timer;
    
    PROCESS_BEGIN();

    steeringEngine_init();    
    etimer_set(&robotArm_timer,50); //设置etimer定时器 
    
    while(1) 
    {     
        steer1_control(V1);
        steer2_control(V2);
        steer3_control(V3);
        steer4_control(V4);
        
        PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
        etimer_reset(&robotArm_timer);
    }

    PROCESS_END();
}

