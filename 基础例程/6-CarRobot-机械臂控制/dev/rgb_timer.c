#include "rgb_timer.h"
extern char V3, V4, V5;
/*********************************************************************************************
* 名称：TIM3_Init
* 功能：初始化TIM3
* 参数：period：自动重装值。 prescaler：时钟预分频数
* 返回：无
* 修改：
* 注释：定时器溢出时间计算方法:Tout=((period+1)*(prescaler+1))/Ft us.
*       AHB Prescaler = 1;AHB的时钟HCLK=SYSCLK/1 = 168MHz;
*       TIM3挂载在APB1上， APB1 Prescaler = 4，APB1的时钟频率PCLK1 = HCLK/4 = 42;Ft=2*PCLK1= 84MHz
*       Ft=定时器工作频率,单位:Mhz，
*********************************************************************************************/
void hardware_pwm_init(unsigned short period,unsigned short prescaler)//TIM_Period为16位的数
{
  GPIO_InitTypeDef GPIO_InitStructure;
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;               //定时器配置
  TIM_OCInitTypeDef  TIM_OCInitStructure;

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); 	//使能GPIOB时钟
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource0,GPIO_AF_TIM3);         //PB0、PB1复用为定时器3
  GPIO_PinAFConfig(GPIOB,GPIO_PinSource1,GPIO_AF_TIM3);         //PB0、PB1复用为定时器3

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;          //PB0、PB1
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;                  //复用功能
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	        //速度100MHz
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                //推挽复用输出
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                  //上拉
  GPIO_Init(GPIOB,&GPIO_InitStructure);                         //初始化PB0、PB1

  TIM_TimeBaseStructure.TIM_Period = period;                    //计数器重装值
  TIM_TimeBaseStructure.TIM_Prescaler = prescaler;              //预分频值
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;       //时钟分割
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   //计数模式
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);               //按上述配置初始化TIM3
  //初始化TIM3 Channel1 PWM模式

  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;             //选择定时器模式:TIM脉冲宽度调制模式2
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;      //输出极性:TIM输出比较极性低
  TIM_OC3Init(TIM3, &TIM_OCInitStructure);                      //根据T指定的参数初始化外设TIM1 4OC1
  TIM_OC4Init(TIM3, &TIM_OCInitStructure);                      //根据T指定的参数初始化外设TIM1 4OC1

  TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);             //使能TIM3在CCR3上的预装载寄存器
  TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);             //使能TIM3在CCR4上的预装载寄存器

  TIM_ARRPreloadConfig(TIM3,ENABLE);                            //ARPE使能
  TIM_Cmd(TIM3, ENABLE);                                        //使能TIM3
}

/*********************************************************************************************
* 名称：TIM4_Init
* 功能：初始化TIM4
* 参数：period：自动重装值。 prescaler：时钟预分频数
* 返回：无
* 修改：
* 注释：定时器溢出时间计算方法:Tout=((period+1)*(prescaler+1))/Ft us.
*       AHB Prescaler = 1;AHB的时钟HCLK=SYSCLK/1 = 168MHz;
*       TIM3挂载在APB1上， APB1 Prescaler = 4，APB1的时钟频率PCLK1 = HCLK/4 = 42;Ft=2*PCLK1= 84MHz
*       Ft=定时器工作频率,单位:Mhz，
*********************************************************************************************/
void software_pwm_init(unsigned short period,unsigned short prescaler)//TIM_Period为16位的数
{
  GPIO_InitTypeDef GPIO_InitStructure;
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;               //定时器配置
  NVIC_InitTypeDef  NVIC_InitStructure;                         //中断配置

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); 	//使能GPIOB时钟

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;                     //PB2
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;                 //输出功能
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	        //速度100MHz
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                //推挽输出
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                  //上拉
  GPIO_Init(GPIOB,&GPIO_InitStructure);                         //初始化PB2

  NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;               //TIM4中断通道
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;       //抢占优先级0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;            //子优先级1
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;               //使能中断
  NVIC_Init(&NVIC_InitStructure);			        //按照上述配置初始化中断

  TIM_TimeBaseStructure.TIM_Period = period;                    //计数器重装值
  TIM_TimeBaseStructure.TIM_Prescaler = prescaler;              //预分频值
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;       //时钟分割
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   //计数模式
  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);               //按上述配置初始化TIM4

  TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);                      //允许定时器4更新中断
  TIM_Cmd(TIM4, ENABLE);                                        //使能TIM4
}

/*********************************************************************************************
* 名称：TIM4_IRQHandler
* 功能：TIM3中断处理函数
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
unsigned int tick = 0;
void TIM4_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM4, TIM_IT_Update ) != RESET) {         //如果中断标志被设置
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);                 //清除中断标志
    tick++;
    if(tick >= V5)
      GPIO_SetBits(GPIOB, GPIO_Pin_2);
    else
      GPIO_ResetBits(GPIOB, GPIO_Pin_2);
    if(tick >= 256)   tick = 0;
  }
}

void rgb_init(void)
{
  hardware_pwm_init(500-1, 84-1);                                       //初始化TIM3，84M/84=1Mhz的计数频率,重装载值500，所以PWM频率为 1M/500=2Khz.
  software_pwm_init(2-1, 84-1);                                         //初始化TIM4，84M/84=1Mhz的计数频率,重装载值500，所以PWM频率为1Mhz.
}