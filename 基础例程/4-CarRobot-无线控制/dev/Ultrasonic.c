#include "Ultrasonic.h"
#include "contiki.h"
#include "delay.h"
#include "motor.h"
#include "stdio.h"
#include "led.h"


TIM_ICInitTypeDef  TIM12_ICInitStructure;

/*********************************************************************************************
* 名称：TIM12CH2_Cap_Init
* 功能：定时器12通道2输入捕获配置
* 参数：arr：自动重装值(TIM2,TIM12是32位的!!)
*       psc：时钟预分频数
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void TIM12CH2_Cap_Init(u32 arr,u16 psc)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM12,ENABLE);  	//TIM12时钟使能    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); 	//使能GPIOB时钟	
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15; //GPIO
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN; //下拉
    GPIO_Init(GPIOB,&GPIO_InitStructure); //初始化GPIO
    
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource15,GPIO_AF_TIM12); //PB15复用位定时器12
    
    TIM_TimeBaseStructure.TIM_Period=arr;   //自动重装载值
    TIM_TimeBaseStructure.TIM_Prescaler=psc;  //定时器分频
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
    
    TIM_TimeBaseInit(TIM12,&TIM_TimeBaseStructure);
    
    //初始化TIM12输入捕获参数
    TIM12_ICInitStructure.TIM_Channel = TIM_Channel_2;                  //CC1S=01 选择输入端 IC2映射到TI2上
    TIM12_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;   //映射到TI1上
    TIM12_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;       //上升沿捕获
    TIM12_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;             //配置输入分频,不分频 
    TIM12_ICInitStructure.TIM_ICFilter = 0x00;                          //IC1F=0000 配置输入滤波器 不滤波
    TIM_ICInit(TIM12, &TIM12_ICInitStructure);
    
    NVIC_InitStructure.NVIC_IRQChannel = TIM8_BRK_TIM12_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//子优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
    
    NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、   
    TIM_ITConfig(TIM12,TIM_IT_Update|TIM_IT_CC2,ENABLE);//允许更新中断 ,允许CC2IE捕获中断	
    TIM_Cmd(TIM12,DISABLE ); 	//失能定时器12    
}

    					
u32 TIM12CH2_CAPTURE_VAL;	//输入捕获值(TIM2/TIM12是32位)
u8  TIM12CH2_CAPTURE_STA=0;	//输入捕获状态	

/*********************************************************************************************
* 名称：TIM8_BRK_TIM12_IRQHandler
* 功能：定时器12中断服务函数
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void TIM8_BRK_TIM12_IRQHandler(void)
{ 		    
    if(TIM_GetITStatus(TIM12, TIM_IT_Update) != RESET)      //溢出
    {	                
        TIM_Cmd(TIM12,DISABLE ); 	                        //关闭定时器   
        TIM_OC2PolarityConfig(TIM12,TIM_ICPolarity_Rising);//CC1P=0 设置为上升沿捕获
        
        if(TIM12CH2_CAPTURE_STA&0x01)                           //检测到信号，但距离太远
        {
            ultrasonicValue = 510;
        }
        else                                                    //没有信号
        {
            ultrasonicValue = 0;
        }
    }
    
    if(TIM_GetITStatus(TIM12, TIM_IT_CC2) != RESET)         //发生捕获事件2
    {	
        if(TIM12CH2_CAPTURE_STA&0x01)		                //捕获到一个上升沿 		
        {	  			
            TIM_Cmd(TIM12,DISABLE ); 	                    //关闭定时器            
            TIM12CH2_CAPTURE_STA = 0x02;		                //标记成功捕获到一次高电平脉宽
            TIM12CH2_CAPTURE_VAL=TIM_GetCapture2(TIM12);        //获取当前的捕获值.
            TIM_OC2PolarityConfig(TIM12,TIM_ICPolarity_Rising);//CC1P=0 设置为上升沿捕获
            
            ultrasonicValue = (TIM12CH2_CAPTURE_VAL *  0.17);//计算距离
        }
        else  								                //还未开始,第一次捕获上升沿
        {
            TIM12CH2_CAPTURE_STA = 0x01;		            //标记捕获到了上升沿
            TIM_Cmd(TIM12,DISABLE ); 	                    //关闭定时器               
            TIM_SetCounter(TIM12,0);
            TIM_OC2PolarityConfig(TIM12,TIM_ICPolarity_Falling);//CC1P=1 设置为下降沿捕获
            TIM_Cmd(TIM12,ENABLE ); 	                    //使能定时器
        }		    
    }			
    
	TIM_ClearITPendingBit(TIM12, TIM_IT_CC2|TIM_IT_Update); //清除中断标志位
}



/*********************************************************************************************
* 名称：Ultrasonic_init
* 功能：超声波初始化
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void Ultrasonic_init(void) 
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    RCC_AHB1PeriphClockCmd(RCC_trig, ENABLE);                     //使能GPIOB时钟
    
    GPIO_InitStructure.GPIO_Pin = trig_pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;                 //输出模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;             //输出速度50MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;                //下拉
    GPIO_Init(trig_port, &GPIO_InitStructure);                    //根据上述参数配置GPIO
    GPIO_ResetBits(trig_port, trig_pin);
    
    
    GPIO_InitStructure.GPIO_Pin = echo_pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;                  //输入模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                //推挽
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;             //输出速度50MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;                //下拉
    GPIO_Init(echo_port, &GPIO_InitStructure);                    //根据上述参数配置GPIO
    
    /*10us计数，定时30ms*/
    TIM12CH2_Cap_Init(3000,839);
}


/*********************************************************************************************
* 名称：trigger_ultrasonic
* 功能：启动一次超声波测距
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void trigger_ultrasonic()
{    
    TIM12CH2_CAPTURE_STA=0;			                    //清空
    TIM12CH2_CAPTURE_VAL=0;                
    TIM12->CNT=0;                                        //将TIM计数寄存器的计数值清零
    TIM_Cmd(TIM12,ENABLE ); 	                        //使能定时器
    
    /*触发时序*/  
    GPIO_SetBits(trig_port, trig_pin);
    delay_us(15);
    GPIO_ResetBits(trig_port, trig_pin);
}



/*定义全局变量ultrasonicValue，单位cm*/
float ultrasonicValue=0;

//申明超声波进程
PROCESS(ultrasonic,"ultrasonic");

/*********************************************************************************************
* 名称: PROCESS_THREAD()
* 功能: 定义进程
* 参数: 
* 返回: 无
* 修改:
* 注释:
*********************************************************************************************/
PROCESS_THREAD(ultrasonic, ev, data)
{
    static struct etimer ultrasonic_timer;
    
    PROCESS_BEGIN();
    
    Ultrasonic_init();
    etimer_set(&ultrasonic_timer,50);                           //设置etimer定时器
    
    while (1) 
    {       
        PROCESS_WAIT_EVENT(); 
        if(ev == PROCESS_EVENT_TIMER)
        {
            etimer_reset(&ultrasonic_timer);                    //重置etimer定时器 

            trigger_ultrasonic();                               //触发一次测距
        }       
    }
    
    PROCESS_END();
}

