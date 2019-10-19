#ifndef _ILI93XX_H_
#define _ILI93XX_H_

#include "delay.h"
#include "stm32f4xx.h"

#define SCREEN_ORIENTATION_LANDSCAPE 1
#define ZXBEE_PLUSE
#define SWAP_RGB (1<<3)

/* lcd 使用PD7脚 NE1功能基地址为0x60000000, PD13引脚A18 作为rs选择*/
#ifdef ZXBEE_PLUSE
#define ILI93xx_REG (*((volatile uint16_t *)(0x60000000)))
#define ILI93xx_DAT (*((volatile uint16_t *)(0x60000000 | (1<<(17+1)))))
#else
//NE4功能基地址为0x6C000000, rs A6
#define ILI93xx_REG (*((volatile uint16_t *)(0x6C000000)))
#define ILI93xx_DAT (*((volatile uint16_t *)(0x6C000000 | (1<<(6+1)))))
#endif

void LCD_WR_REG(vu16 regval);
void LCD_WR_DATA(vu16 data);
u16 LCD_RD_DATA(void);
void ILI93xx_WriteReg(uint16_t r, uint16_t d);
uint16_t ILI93xx_ReadReg(uint16_t r);
void BLOnOff(int st);
void BLInit(void);
void ILI93xxInit(void);
void LCDSetWindow(int x, int xe, int y, int ye);
void LCDSetCursor(int x, int y);
void LCDWriteData(uint16_t *dat, int len);
void LCDClear(uint16_t color);
void LCDDrawPixel(int x, int y, uint16_t color);
void LCDrawLineH(int x0, int x1, int y0, int color);
void LCDDrawAsciiDot12x24_1(int x, int y, char ch, int color, int bc);
void LCDDrawAsciiDot12x24(int x, int y, char *str, int color, int bc);
void LCDDrawAsciiDot8x16_1(int x, int y, char ch, int color, int bc);
void LCDDrawAsciiDot8x16(int x, int y, char *str, int color, int bc);
void LCDDrawGB_16_1(int x, int y, char *gb2, int color, int bc) ;
void LCDDrawGB_24_1(int x, int y, char *gb2, int color, int bc) ;
void LCDDrawFnt16(int x, int y, char *str, int color, int bc);
void LCDDrawFnt24(int x, int y, char *str, int color, int bc);
void LCD_Clear(int x1,int y1,int x2,int y2,uint16_t color);
void LCD_ShowPic(int x,int y,int length,int height,unsigned char *pic);

/*********************************************************************************************
* 名称：LCD_PutChar16
* 功能：16*16汉字字符显示（文件字库版）
* 参数：x，y：显示坐标 *data：字符串名Color：汉字颜色 BColor：背景颜色
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void LCD_PutChar16(u16 x,u16 y,u8 *data,u16 Color,u16 BColor);

/*********************************************************************************************
* 名称：lcdShowFont16
* 功能：lcd显示16x16汉字
* 参数：x1,y1:起始坐标值，color：字体颜色，Bcolor：背景颜色
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void lcdShowFont16(uint16 x,uint16 y,char* p,uint16 color,uint16 Bcolor);

#endif
