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
* 参数：angle:舵机转动角度，20-180,
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
* 参数：angle:舵机转动角度，0-180,
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
* 参数：angle:舵机转动角度，0-180,
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


/*********************************************************************************************
机械臂控制参数
*********************************************************************************************/
_robotArm robotArm = {6,10,90,0};




/*********************************************************************************************
机械臂动作事件
*********************************************************************************************/
process_event_t robotArm_motion_event;



/*声明进程*/
PROCESS(robotArm_motion,"robotArm_motion");

/*********************************************************************************************
* 名称: PROCESS_THREAD()
* 功能: 定义进程：机械臂自定义动作
* 参数: ev:事件，data:事件所带的数据
* 返回: 无
* 修改:
* 注释:
*********************************************************************************************/
PROCESS_THREAD(robotArm_motion, ev, data)
{   
    static struct etimer robotArm_motion_t;
    
    PROCESS_BEGIN();   
    
    while(1)
    {      
        PROCESS_WAIT_EVENT();           
        if(ev == robotArm_motion_event)
        {
            /*自检动作*/
            if((*(uint8*)data)==1)
            {   
                /*3,4复位*/
                V5 = 90;
                V6 = 0;
                etimer_set(&robotArm_motion_t,500); //设置etimer定时器
                PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
                
                /*1,2上移*/            
                V3 = 10;
                V4 = 30;
                etimer_set(&robotArm_motion_t,3000); //设置etimer定时器
                PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
                
                /*4张开*/
                V6 = 70;
                etimer_set(&robotArm_motion_t,500); //设置etimer定时器
                PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
                
                /*3逆时针*/
                V5 = 0;
                etimer_set(&robotArm_motion_t,500); //设置etimer定时器
                PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
                
                /*3顺时针*/
                V5 = 180;
                etimer_set(&robotArm_motion_t,1000); //设置etimer定时器
                PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
                
                /*3逆时针*/
                V5 = 90;
                etimer_set(&robotArm_motion_t,500); //设置etimer定时器
                PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
              
                /*4闭合*/
                V6 = 0;
                etimer_set(&robotArm_motion_t,500); //设置etimer定时器
                PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
           
                /*复位*/
                V3 = 6;
                V4 = 10;
            }
            /*倒水动作*/
            else if(*(uint8*)data==2)
            {               
                /*3,4复位*/
                V5 = 90;
                V6 = 0;
                etimer_set(&robotArm_motion_t,1000); //设置etimer定时器
                PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
                
                /*4张开 1,2前伸*/
                V6 = 70;
                V3 = 17;
                V4 = 5;
                etimer_set(&robotArm_motion_t,2500); //设置etimer定时器
                PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
                
                /*4闭合*/
                V6 = 50;
                etimer_set(&robotArm_motion_t,1000); //设置etimer定时器
                PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
                
                /*1,2上移*/
                V3 = 17;
                V4 = 25;
                etimer_set(&robotArm_motion_t,2000); //设置etimer定时器
                PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
                
                /*3逆时针*/
                V5 = 0;
                etimer_set(&robotArm_motion_t,1500); //设置etimer定时器
                PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
                
                /*3顺时针*/
                V5 = 90;
                etimer_set(&robotArm_motion_t,500); //设置etimer定时器
                PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
                
                /*1,2下移*/
                V3 = 17;
                V4 = 5;
                etimer_set(&robotArm_motion_t,1500); //设置etimer定时器
                PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
                
                /*4张开*/
                V6 = 70;
                etimer_set(&robotArm_motion_t,1000); //设置etimer定时器
                PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
                
                /*复位*/
                V3 = 6;
                V4 = 10;
                etimer_set(&robotArm_motion_t,1000); //设置etimer定时器
                PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
                
                /*4闭合*/
                V6 = 0;
            }
            /*捡垃圾*/
            else if(*(uint8*)data==3)
            {               
                /*3,4复位*/
                V5 = 90;
                V6 = 0;
                etimer_set(&robotArm_motion_t,1000); //设置etimer定时器
                PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
                
                /*4张开 1,2下移*/
                V6 = 70;
                V3 = 5;
                V4 = 2;
                etimer_set(&robotArm_motion_t,2000); //设置etimer定时器
                PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);  
                
                /*4闭合*/
                V6 = 20;
                etimer_set(&robotArm_motion_t,500); //设置etimer定时器
                PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);      
                
                /*1,2上升*/
                V3 = 8;
                for(;V4<27;V4++)
                {
                    etimer_set(&robotArm_motion_t,100); //设置etimer定时器
                    PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);  
                }                
                
                /*1,2前移*/
                V3 = 17;
                etimer_set(&robotArm_motion_t,2000); //设置etimer定时器
                PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);  
                              
                /*4张开*/
                V6 = 70;
                etimer_set(&robotArm_motion_t,1000); //设置etimer定时器
                PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);      
                
                /*复位*/
                V3 = 6;
                V4 = 10;
                V6 = 0;               
            }
            /*扔东西*/
            else if(*(uint8*)data==4)
            {
                /*3,4复位*/
                V5 = 90;
                V6 = 0;
                etimer_set(&robotArm_motion_t,1000); //设置etimer定时器
                PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
                
                /*4张开 1,2下移*/
                V6 = 70;
                V3 = 7;
                V4 = 2;
                etimer_set(&robotArm_motion_t,2000); //设置etimer定时器
                PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);  
                
                /*4闭合*/
                V6 = 20;
                etimer_set(&robotArm_motion_t,500); //设置etimer定时器
                PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);      
                
                /*扔出*/
                TIM_Cmd(TIM5,DISABLE);                          //关闭定时器 
                V3 = 17;
                V4 = 28;
                etimer_set(&robotArm_motion_t,1000); //设置etimer定时器
                PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);  
                STEER1 = steer1Pwm;
                STEER2 = steer2Pwm; 
                etimer_set(&robotArm_motion_t,300); //设置etimer定时器
                PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);  
                V6 = 70;   
                etimer_set(&robotArm_motion_t,1000); //设置etimer定时器
                PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);  
                TIM_Cmd(TIM5,ENABLE);                          //打开定时器 
                
                /*复位*/
                V3 = 6;
                V4 = 10;
                V6 = 0;
            }
            V7 = 0;
        }
    }
    PROCESS_END();
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
    double a1=0,a2=0;
    double x1=0,y1=0,temp1=0,temp2=0;
    static struct etimer robotArm_timer;
    
    PROCESS_BEGIN();

    steeringEngine_init();     
//    robotArm_motion_event = process_alloc_event();
    V7 = 1;                                                     //设置自检事件
    etimer_set(&robotArm_timer,100); //设置etimer定时器
    
    while(1) 
    {                                    
        PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
        etimer_reset(&robotArm_timer);  
    
        /*取值范围*/
        temp1 = L1+L2;
        if(robotArm.x<1) robotArm.x=1;
        if(robotArm.x>temp1) robotArm.x=temp1;
        if(robotArm.y<1) robotArm.y=1;
        if(robotArm.y>temp1+H) robotArm.y=temp1+H;
        
        x1 = L1/sqrt(2);
        y1 = x1+H;
        if( (((robotArm.x+x1)*(robotArm.x+x1)+(robotArm.y-y1)*(robotArm.y-y1))>=(L2*L2))
           && (robotArm.x*robotArm.x+((robotArm.y-H)*(robotArm.y-H)))<=((L1+L2)*(L1+L2)) )
        {        
            /*计算弧度*/
            temp1 = ((L1*L1)+(robotArm.x*robotArm.x)+((robotArm.y-H)*(robotArm.y-H))-(L2*L2))/(2*L1*sqrt((robotArm.x*robotArm.x)+((robotArm.y-H)*(robotArm.y-H))));
            temp2 = (H-robotArm.y)/sqrt((robotArm.x*robotArm.x)+((robotArm.y-H)*(robotArm.y-H)));        
            a1 = pi-acos(temp1)-acos(temp2);
            a2 = acos(((L1*L1)+(L2*L2)-(robotArm.x*robotArm.x)-((robotArm.y-H)*(robotArm.y-H)))/(2*L1*L2));
            
            /*弧度转换为角度*/
            a1 = a1*(180.0/pi) + 40;
            a2 = a2*(180.0/pi);
            
            steer1_control(a1);
            steer2_control(a2);
            steer3_control(robotArm.angle_3);
            steer4_control(robotArm.angle_4);
        }
    }

    PROCESS_END();
}
//1 --> 20-180
//2 --> 175-0

