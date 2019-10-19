#include <stdio.h>
#include <contiki.h>
#include <dev/leds.h>
#include "ili93xx.h"
#include "adc.h"
#include "tim.h"
#include "usart.h"
#include "string.h"
#include "rtc.h"
#include "sample.h"
#include "user.h"




void fsmc_init(void);

extern char qrcode_flag;
extern u8 motion_mode;
extern u8 motor_direction;

void drawing_window(uint16 x,uint16 y,uint16 brush_color,uint16 backdrop_color)
{ 
    LCD_Clear(x, y, x+64, y+20,brush_color);
    LCD_Clear(x+64+5, y, x+140, y+20,brush_color);
    LCD_Clear(x+64+5+1, y+1, x+140-1, y+20-1,backdrop_color);
}


void lcd_page1(int page)
{
    uint16 temp_x=0,temp_y=0;
    uint16 brush_color=0xffff;          //画笔颜色
    uint16 backdrop_color=0x001F;       //背景颜色
    
    if(page==1) 
    {
        LCDClear(0x0000); 
        
        LCD_Clear(0, 0, 319, 30, backdrop_color);
        LCDDrawFnt24(160-(strlen("车型机器人")*12/2), 3, "车型机器人", brush_color, backdrop_color);
        
        temp_x = 10;
        temp_y = 40;
        drawing_window(temp_x,temp_y,backdrop_color,0x0000);
        LCDDrawFnt16((32-strlen("温度")*8/2)+temp_x, temp_y+2, "温度", brush_color, backdrop_color);
        
        temp_y += 40;
        drawing_window(temp_x,temp_y,backdrop_color,0x0000);
        LCDDrawFnt16((32-strlen("经度")*8/2)+temp_x, temp_y+2, "经度", brush_color, backdrop_color);
        
        temp_y += 40;
        drawing_window(temp_x,temp_y,backdrop_color,0x0000);
        LCDDrawFnt16((32-strlen("光照度")*8/2)+temp_x, temp_y+2, "光照度", brush_color, backdrop_color);
        
        temp_y += 40;
        drawing_window(temp_x,temp_y,backdrop_color,0x0000);
        LCDDrawFnt16((32-strlen("连接模式")*8/2)+temp_x, temp_y+2, "连接模式", brush_color, backdrop_color);
        
        temp_x = 170;
        temp_y = 40;
        drawing_window(temp_x,temp_y,backdrop_color,0x0000);
        LCDDrawFnt16((32-strlen("湿度")*8/2)+temp_x, temp_y+2, "湿度", brush_color, backdrop_color);
        
        temp_y += 40;
        drawing_window(temp_x,temp_y,backdrop_color,0x0000);
        LCDDrawFnt16((32-strlen("纬度")*8/2)+temp_x, temp_y+2, "纬度", brush_color, backdrop_color);
        
        temp_y += 40;
        drawing_window(temp_x,temp_y,backdrop_color,0x0000);
        LCDDrawFnt16((32-strlen("时间")*8/2)+temp_x, temp_y+2, "时间", brush_color, backdrop_color);
        
        temp_y += 40;
        drawing_window(temp_x,temp_y,backdrop_color,0x0000);
        LCDDrawFnt16((32-strlen("控制模式")*8/2)+temp_x, temp_y+2, "控制模式", brush_color, backdrop_color);
        
        LCD_Clear(10, 190, 310, 210,0X8430);
        LCD_Clear(0, 240-20, 319, 240,0x001F);
    }
}




void lcd_page2()
{
    char s[255];
    uint16 temp_x=0,temp_y=0;
    
    //温度
    temp_x = 70+1+10;
    temp_y = 40+3;
    sprintf(s, "%.1f", A1);               
    LCDDrawFnt16(temp_x, temp_y, s, 0xffff, 0x0000);
    
    //经度
    temp_y+=40;
    sprintf(s, "%.4f",A3);
    LCDDrawFnt16(temp_x, temp_y, s, 0xffff, 0x0000);
    
    //光照度
    temp_y+=40;
    sprintf(s, "%.1f",A0);
    LCDDrawFnt16(temp_x, temp_y, s, 0xffff, 0x0000);
    
    //连接模式
    temp_y+=40;
    if(connect_mode==0)
        sprintf(s, "BLE");
    else
        sprintf(s, "GSM");
    LCDDrawFnt16(temp_x, temp_y, s, 0xffff, 0x0000);
    
    //湿度
    temp_x = 70+1+170;
    temp_y = 40+3;
    sprintf(s, "%.1f", A2);
    LCDDrawFnt16(temp_x, temp_y, s, 0xffff, 0x0000);
    
    //纬度
    temp_y+=40;
    sprintf(s, "%.4f",A4);
    LCDDrawFnt16(temp_x, temp_y, s, 0xffff, 0x0000);
    
    //时间
    temp_y+=40;
    sprintf(s, "%02d:%02d:%02d", RTC_TimeStruct.RTC_Hours,RTC_TimeStruct.RTC_Minutes,RTC_TimeStruct.RTC_Seconds);
    LCDDrawFnt16(temp_x, temp_y, s, 0xffff, 0x0000);
    
    
    //控制模式
    temp_y+=40;
    if(motion_mode==1)
    {
        sprintf(s, "手动");        
    }
    else if(motion_mode==2)
    {    
        sprintf(s, "寻迹");
    }
    else if(motion_mode==3)
    {
        sprintf(s, "避障");
    }
    else
    {
        sprintf(s, "停止");
    }
    LCDDrawFnt16(temp_x, temp_y, s, 0xffff, 0x0000);
    
    if (strlen(RADIO_MAC) > 0 ) 
    {
        sprintf(s, "MAC: %s", RADIO_MAC);
        LCDDrawFnt16(65, 222, s, 0x0000, 0x001F);
    }
    
    LCD_Clear(10, 190, 310, 210,0X8430);
    for(uint8 i=0;i<255;i++) s[i]=0;
    //显示信息
    if(V7!=0)
    {
        if(V7==1)
        {           
            sprintf(s, "机械臂自检");
        }
        else if(V7==2)
        {
            sprintf(s, "倒水");
        }
        else if(V7==3)
        {
            sprintf(s, "捡垃圾");
        }
        else if(V7==4)
        {
            sprintf(s, "扔东西");
        }
        else
        {
            sprintf(s, "无动作");
        }
    }
    else if(motion_mode==1)
    {
        if(motor_direction==1)
        {       
            sprintf(s, "前进");   
        }
        else if(motor_direction==2)
        {
            sprintf(s, "后退");
        }
        else if(motor_direction==3)
        {
            sprintf(s, "左转");
        }
        else if(motor_direction==4)
        {
            sprintf(s, "右转");
        }
        else
        {  
            sprintf(s, "停止");
        }
    }
    LCDDrawFnt16(160-(strlen(s)*8/2), 190+2, s, 0x0000, 0X8430);
}



PROCESS(lcd, "lcd");

PROCESS_THREAD(lcd, ev, data)
{
    static struct etimer lcd_timer;
    
    PROCESS_BEGIN();
    
    fsmc_init();                                                //fsmc初始化
    BLInit();                                                   //背光初始化
    ILI93xxInit();                                              //LCD初始化
    lcd_page1(1);                                               //显示page2    
    etimer_set(&lcd_timer,500);                                 //设置lcd_timer定时器                 
    
    while(1)
    {                       
        PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);   //等待lcd_timer定时器溢出   
        etimer_reset(&lcd_timer);                              //设置lcd_timer定时器  
        
        if(qrcode_flag == 0)
        {
            lcd_page2();  
        }
    }
    PROCESS_END();
}