/*********************************************************************************************
* 文件: led.c
* 作者：liutong 2015.8.19
* 说明：LED驱动程序
*       D1->PA5;D2->PA7;D3->PE2;D4->PE3;R->PB0;G->PB1;B->PB2
* 修改：
* 注释：
*********************************************************************************************/
#include "led.h"
#include "rgb_timer.h"


/*********************************************************************************************
* 名称：led_init
* 功能：初始化LED对应的GPIO
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void led_init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE |
                         RCC_AHB1Periph_GPIOA , ENABLE);        //使能GPIO时钟
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;        //选中2、3号引脚
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;                 //输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                //推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;              //输出速度2MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;              //无上下拉
  GPIO_Init(GPIOE, &GPIO_InitStructure);                        //根据上述参数配置GPIOE2、GPIOE3
  GPIO_SetBits(GPIOE, GPIO_Pin_2 | GPIO_Pin_3);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;        //选中5、7号引脚
  GPIO_Init(GPIOA, &GPIO_InitStructure);                        //根据上述参数配置GPIOA5、GPIOA7
  GPIO_SetBits(GPIOA, GPIO_Pin_5 | GPIO_Pin_7);
}

/*********************************************************************************************
* 名称：turn_on_off
* 功能：置引脚低电平，打开LED
* 参数：led
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void turn_on_off(unsigned char led){
  if(led & LED1)
    GPIO_ResetBits(GPIOE, GPIO_Pin_2);
  else
    GPIO_SetBits(GPIOE, GPIO_Pin_2);
  if(led & LED2)
    GPIO_ResetBits(GPIOE, GPIO_Pin_3);
  else
    GPIO_SetBits(GPIOE, GPIO_Pin_3);
  if(led & LED3)
    GPIO_ResetBits(GPIOA, GPIO_Pin_5);
  else
    GPIO_SetBits(GPIOA, GPIO_Pin_5);
  if(led & LED4)
    GPIO_ResetBits(GPIOA, GPIO_Pin_7);
  else
    GPIO_SetBits(GPIOA, GPIO_Pin_7);
}
/*********************************************************************************************
* 名称：turn_on
* 功能：置引脚低电平，打开LED
* 参数：led
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
unsigned char get_led_status(void){
  unsigned char led_status = 0;
  if(GPIO_ReadOutputDataBit(GPIOE, GPIO_Pin_2))
    led_status |= LED1;
  else
    led_status &= ~LED1;
  
  if(GPIO_ReadOutputDataBit(GPIOE, GPIO_Pin_3))
    led_status |= LED2;
  else
    led_status &= ~LED2;
  
  if(GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_5))
    led_status |= LED3;
  else
    led_status &= ~LED3;
  
  if(GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_7))
    led_status |= LED4;
  else
    led_status &= ~LED4;
  return led_status;
}


/*********************************************************************************************
声明进程
*********************************************************************************************/
PROCESS(led,"led");

/*********************************************************************************************
* 名称: PROCESS_THREAD()
* 功能: 定义进程
* 参数: 
* 返回: 无
* 修改:
* 注释:
*********************************************************************************************/
PROCESS_THREAD(led, ev, data)
{
    static struct etimer led_timer;
    
    PROCESS_BEGIN();
    led_init();
    etimer_set(&led_timer,1000);
    
    while (1) 
    {
        PROCESS_WAIT_EVENT();                                       //等待事件发生
            
        if(ev == PROCESS_EVENT_TIMER)
        {
            etimer_reset(&led_timer);                             //重置定时器            
            led1 = !led1;
        }      
    }
    
    PROCESS_END();
}