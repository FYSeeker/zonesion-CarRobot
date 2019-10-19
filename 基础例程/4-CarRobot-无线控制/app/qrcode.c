#include <contiki.h>
#include <contiki-net.h>
#include "adc_key.h"
#include <stdio.h>
#include "ili93xx.h"
#include "qr_encode.h"
#include "gsm.h"
#include "user.h"

#define QRCODE_Y 	0		//TFT二维码显示坐标y

//二维码显示标志：0-->未显示，1-->显示中
char qrcodeShow_flag = 0;

extern void lcd_page1(int page);
extern void lcd_page2();

//0 0 5 5
//6 6 11 11
u8 codetest[]={//微信名片，可以去百度上搜索二维码名片 查看
    //"BEGIN:VCARD\r\n"
    //"VERSION:3.0\r\n"
    //"N:外星人\r\n"//名字
    //"TEL:13888888888\r\n"//手机号
    //"TEL;TYPE=WORK,VOICE:0571-88888888\r\n"//工作号码
    //"TEL;TYPE=HOME,VOICE:0571-66666666\r\n"//家庭号码
    //"NOTE:QQ:12345\r\n"//QQ号码
    //"EMAIL:12345@qq.com\r\n"//邮箱地址
    //"ORG:公司名称\r\n"//公司
    //"TITLE:职位名称\r\n"
    //"URL:http://个人主页\r\n"
    //"ADR;TYPE=WORK:工作地址\r\n"//地址
    //"ADR;TYPE=HOME:家庭住址\r\n"
    //"END:VCARD"
    "0123456789"
};

void DISPLAY_RENCODE_TO_TFT(u8 *qrcode_data)
{
    u8 i,j;
    u16 x,y,p;
    char s[64];
    EncodeData((char *)qrcode_data);
    LCDClear(0);
    if(m_nSymbleSize*2>240)	
    {
        return;
    }
    if(m_nSymbleSize < 1)	
    {
        sprintf(s, "MAC地址获取失败");
        LCDDrawFnt16((320-7*16-8)/2, (240-16)/2, s, 0x0000, 0xffff);
    }
    for(i=0;i<10;i++)
    {
        if((m_nSymbleSize*i*2)>240)	break;
    }
    p=i;//点大小
    x=(320-m_nSymbleSize*p)/2;
    y=(240-m_nSymbleSize*p)/2;
    LCD_Clear(x-2,y-2,x+m_nSymbleSize*p+2,y+m_nSymbleSize*p+2,0xffff); 
    for(i=0;i<m_nSymbleSize;i++)
    {
        for(j=0;j<m_nSymbleSize;j++)
        {
            if(m_byModuleData[i][j]==1)
                LCD_Clear(x+p*i,y+p*j,x+p*(i+1)-1,y+p*(j+1),0); 
        }
        
    }
}



PROCESS(qrcode, "qrcode");


PROCESS_THREAD(qrcode, ev, data)
{
    PROCESS_BEGIN();
    while (1) 
    {
        PROCESS_WAIT_EVENT();                                   //等待事件发生
        if(ev == process_event_key)                             //判断是否发生按键事件
        {                                
            if((*(int*)data)==KEY1_LONG_PRESSED)                //判断事件传入的值
            {                     
                qrcodeShow_flag = 1;
                if(connect_mode==1)
                    DISPLAY_RENCODE_TO_TFT((unsigned char *)gsm_info.imei);
                else
                    DISPLAY_RENCODE_TO_TFT((unsigned char *)RADIO_MAC);
            }
            if((*(int*)data)==KEY2_PRESSED)                     //判断事件传入的值
            {                          
                qrcodeShow_flag = 0;
                lcd_page1(1);
                lcd_page2();
            }
        } 
    }
    PROCESS_END();
}