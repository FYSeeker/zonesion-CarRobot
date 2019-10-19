#include <stdio.h>
#include <contiki.h>
#include "ili93xx.h"
#include "string.h"
#include "rtc.h"
#include "sample.h"
#include "user.h"


//lcd颜色参数
#define LCD_COLOR_BLACK 0x0000
#define LCD_COLOR_WHITE 0xFFFF
#define LCD_COLOR_GRAY  0x8430
#define LCD_COLOR_RED   0xF800
#define LCD_COLOR_GREEN 0x07E0
#define LCD_COLOR_BLUE  0x001F

void fsmc_init(void);

//绘制显示窗口
void drawing_window(uint16 x,uint16 y,uint16 brush_color,uint16 backdrop_color)
{ 
    LCD_Clear(x, y, x+64, y+20,brush_color);
    LCD_Clear(x+64+5, y, x+140, y+20,brush_color);
    LCD_Clear(x+64+5+1, y+1, x+140-1, y+20-1,backdrop_color);
}


void lcd_page1(int page)
{
    uint16 temp_x=0,temp_y=0;
    uint16 brush_color=LCD_COLOR_WHITE;             //画笔颜色
    uint16 backdrop_color=LCD_COLOR_BLUE;           //背景颜色
    
    if(page==1) 
    {
        LCDClear(LCD_COLOR_BLACK); 
        
        LCD_Clear(0, 0, 319, 30, backdrop_color);
        LCDDrawFnt24(160-(strlen("车型机器人")*12/2), 3, "车型机器人", brush_color, backdrop_color);
        
        temp_x = 10;
        temp_y = 60;
        drawing_window(temp_x,temp_y,backdrop_color,LCD_COLOR_BLACK);
        LCDDrawFnt16((32-strlen("温度")*8/2)+temp_x, temp_y+2, "温度", brush_color, backdrop_color);
        
        temp_y += 40;
        drawing_window(temp_x,temp_y,backdrop_color,LCD_COLOR_BLACK);
        LCDDrawFnt16((32-strlen("光照度")*8/2)+temp_x, temp_y+2, "光照度", brush_color, backdrop_color);
        
        temp_x = 170;
        temp_y = 60;
        drawing_window(temp_x,temp_y,backdrop_color,LCD_COLOR_BLACK);
        LCDDrawFnt16((32-strlen("湿度")*8/2)+temp_x, temp_y+2, "湿度", brush_color, backdrop_color);
        
        temp_y += 40;
        drawing_window(temp_x,temp_y,backdrop_color,LCD_COLOR_BLACK);
        LCDDrawFnt16((32-strlen("时间")*8/2)+temp_x, temp_y+2, "时间", brush_color, backdrop_color);
        
        LCD_Clear(10, 160, 310, 180,LCD_COLOR_GRAY);
        LCD_Clear(0, 240-20, 319, 240,LCD_COLOR_BLUE);
    }
}




void lcd_page2()
{
    char str[255];
    uint16 temp_x=0,temp_y=0;
    
    //温度
    temp_x = 70+1+10;
    temp_y = 60+3;
    sprintf(str, "%04.1f", A1);               
    LCDDrawFnt16(temp_x, temp_y, str, 0xffff, 0x0000);

    //光照度
    temp_y+=40;
    sprintf(str, "%06.1f",A0);
    LCDDrawFnt16(temp_x, temp_y, str, 0xffff, 0x0000);
    
    //湿度
    temp_x = 70+1+170;
    temp_y = 60+3;
    sprintf(str, "%04.1f", A2);
    LCDDrawFnt16(temp_x, temp_y, str, 0xffff, 0x0000);
    
    //时间
    temp_y+=40;
    sprintf(str, "%02d:%02d:%02d",RTC_TimeStruct.RTC_Hours,RTC_TimeStruct.RTC_Minutes,RTC_TimeStruct.RTC_Seconds);
    LCDDrawFnt16(temp_x, temp_y, str, 0xffff, 0x0000);
    
    //显示信息
    sprintf(str,"信息采集实验");
    lcdShowFont16(160-(strlen(str)*8/2), 160+2, str, LCD_COLOR_RED, LCD_COLOR_GRAY);
    
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
        
        lcd_page2();  
    }
    PROCESS_END();
}