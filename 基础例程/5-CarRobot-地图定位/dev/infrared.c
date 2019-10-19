/******************************************************
*文件: infrared.c
*作者: liding 2015.9.22
*说明: 人体红外传感器实验
*修改:
*注释:
*******************************************************/
#include "infrared.h"

/*********************************************************************************************
* 名称：GPIO_init
* 功能：功能初始化
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void infrared_init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);         //使能GPIOD时钟

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;                     //PD3
  GPIO_InitStructure.GPIO_Speed =  GPIO_High_Speed;             //高速
  GPIO_InitStructure.GPIO_Mode  =  GPIO_Mode_IN;                //输入
  GPIO_InitStructure.GPIO_OType =  GPIO_OType_OD;               //开漏模式
  GPIO_InitStructure.GPIO_PuPd  =  GPIO_PuPd_DOWN ;             //下拉

  GPIO_Init(GPIOD, &GPIO_InitStructure);                        //初始化

}

/*********************************************************************************************
* 名称：Get_State
* 功能：获取引脚的电平值
* 参数：GPIO_TypeDef* GPIOx GPIO组 uint16_t GPIO_Pin 引脚号
* 返回：电平状态
* 修改：
* 注释：
*********************************************************************************************/
int infrared_get(void)
{

  if(GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_3)){  //读取PD3的电平
    return 1;
  }
  else{
    return 0;
  }
}
