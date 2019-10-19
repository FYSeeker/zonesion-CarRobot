#include "E18-D80NK.h"
#include "contiki.h"
/*********************************************************************************************
* 名称：E18D80NK_init
* 功能：初始化红外对应的GPIO
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void E18D80NK_init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  RCC_AHB1PeriphClockCmd(RCC_D80NK , ENABLE);                   //使能GPIOB时钟
  
  GPIO_InitStructure.GPIO_Pin = pin_left | pin_right;           //选中PB8、PB9引脚
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;                  //输入模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                //推挽
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;             //速度50MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;              //无上下拉
  GPIO_Init(left_port, &GPIO_InitStructure);                    //根据上述参数配置GPIO
}

u8 get_state(void)
{
  char state=0x00;
  
  if(GPIO_ReadInputDataBit(left_port,pin_left)==1)
    state |=0x01;
  if(GPIO_ReadInputDataBit(right_port,pin_right)==1)
    state |=0x02;
  
  return state;
}