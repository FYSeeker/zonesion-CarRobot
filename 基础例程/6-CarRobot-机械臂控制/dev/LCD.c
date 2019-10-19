#include "lcd.h"
#include "font.h"
#include "delay.h"
#include "8X16.h"
#include "GB3232.h"	//32*32汉字字模
#include "w25qxx.h"

extern char sj[10],cm;
extern u16 sjx,sjy;

/***********************************************************************
*函数名： LCD_Config
*函数功能：LCD接口配置及FSMC配置
*入口参数：无
***********************************************************************/

void  LCD_Config(void)
{ 
    GPIO_InitTypeDef GPIO_InitStructure;
	
	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
    FSMC_NORSRAMTimingInitTypeDef  readWriteTiming; 
    FSMC_NORSRAMTimingInitTypeDef  writeTiming;
    
    /* Enable FSMC, GPIOD, GPIOE, GPIOF, GPIOG and AFIO clocks */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);
    
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE |
                            RCC_APB2Periph_GPIOF | RCC_APB2Periph_GPIOG |
                            RCC_APB2Periph_AFIO, ENABLE);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_SetBits(GPIOA, GPIO_Pin_1);
    
    /* Set PD.00(D2), PD.01(D3), PD.04(NOE), PD.05(NWE), PD.08(D13), PD.09(D14),
    PD.10(D15), PD.14(D0), PD.15(D1) as alternate 
    function push pull */
    GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 |
                                    GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_14 | 
                                    GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    /* Set PE.07(D4), PE.08(D5), PE.09(D6), PE.10(D7), PE.11(D8), PE.12(D9), PE.13(D10),
    PE.14(D11), PE.15(D12) as alternate function push pull */
    GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | 
                                    GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | 
                                    GPIO_Pin_15;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    
    /* Set PF.00(A0 (RS)) as alternate function push pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_Init(GPIOF, &GPIO_InitStructure);
    
    /* Set PG.12(NE4 (LCD/CS)) as alternate function push pull - CE3(LCD /CS) */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_Init(GPIOG, &GPIO_InitStructure);
    
    
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC,ENABLE);	//使能FSMC时钟
    readWriteTiming.FSMC_AddressSetupTime = 0x01;	 //地址建立时间（ADDSET）为2个HCLK 1/36M=27ns
    readWriteTiming.FSMC_AddressHoldTime = 0x00;	 //地址保持时间（ADDHLD）模式A未用到	
    readWriteTiming.FSMC_DataSetupTime = 0x0f;		 // 数据保存时间为16个HCLK,因为液晶驱动IC的读数据的时候，速度不能太快，尤其对1289这个IC。
    readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
    readWriteTiming.FSMC_CLKDivision = 0x00;
    readWriteTiming.FSMC_DataLatency = 0x00;
    readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_A;	 //模式A 
    
    
    writeTiming.FSMC_AddressSetupTime = 0x02;	 //0x01 地址建立时间（ADDSET）为1个HCLK  
    writeTiming.FSMC_AddressHoldTime = 0x00;	 //地址保持时间（A		
    writeTiming.FSMC_DataSetupTime = 0x05;		 ////0x03 数据保存时间为4个HCLK	
    writeTiming.FSMC_BusTurnAroundDuration = 0x00;
    writeTiming.FSMC_CLKDivision = 0x00;
    writeTiming.FSMC_DataLatency = 0x00;
    writeTiming.FSMC_AccessMode = FSMC_AccessMode_A;	 //模式A 
    
    
    FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM4;//  这里我们使用NE4 ，也就对应BTCR[6],[7]。
    FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable; // 不复用数据地址
    FSMC_NORSRAMInitStructure.FSMC_MemoryType =FSMC_MemoryType_SRAM;// FSMC_MemoryType_SRAM;  //SRAM   
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;//存储器数据宽度为16bit   
    FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode =FSMC_BurstAccessMode_Disable;// FSMC_BurstAccessMode_Disable; 
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
    FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait=FSMC_AsynchronousWait_Disable; 
    FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;   
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;  
    FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;	//  存储器写使能
    FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;   
    FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable; // 读写使用不同的时序
    FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable; 
    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming; //读写时序
    FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &writeTiming;  //写时序
    
    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);  //初始化FSMC配置
    
    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);  // 使能BANK1
    
}

/***********************************************************************
*函数名： LCD_Init
*函数功能：LCD初始化
*入口参数：无
***********************************************************************/

void LCD_Init(void)
{
    LCD_Config();
    delay_ms(50); 					      // delay 50 ms 
    LCD_WriteReg_Only(0x0011);    //退出睡眠模式，0x0010为进入
    delay_ms(50); 
    LCD_WriteReg_Only(0x00D0);    //设置电源参数，后续3个参数
    LCD_WriteRAM(0x07);    
    LCD_WriteRAM(0x42);      
    LCD_WriteRAM(0x18);    
    
    LCD_WriteReg_Only(0xD1);      //Vcom Control 后续三个参数
    LCD_WriteRAM(0x00);    
    LCD_WriteRAM(0x07);   
    LCD_WriteRAM(0x10);    
    
    LCD_WriteReg_Only(0xD2);     //设置普通模式下的电源参数，后续两个参数
    LCD_WriteRAM(0x01);    
    LCD_WriteRAM(0x02);    
    
    LCD_WriteReg_Only(0xC0);    //Panel Driving setting  后续5个参数
    LCD_WriteRAM(0x10);         //设置扫描模式
    LCD_WriteRAM(0x3B);         //设定行数480行
    LCD_WriteRAM(0x00);    
    LCD_WriteRAM(0x02);    
    LCD_WriteRAM(0x11);    
    
    LCD_WriteReg(0xC5,0x03);    //Frame Rate and Inversion Control 后续一个参数
    
    
    LCD_WriteReg_Only(0xC8);    //伽马校正,后续12个参数
    LCD_WriteRAM(0x00);    
    LCD_WriteRAM(0x32);    
    LCD_WriteRAM(0x36);    
    LCD_WriteRAM(0x45);    
    LCD_WriteRAM(0x06);    
    LCD_WriteRAM(0x16);    
    LCD_WriteRAM(0x37);    
    LCD_WriteRAM(0x75);    
    LCD_WriteRAM(0x77);    
    LCD_WriteRAM(0x54);    
    LCD_WriteRAM(0x0C);    
    LCD_WriteRAM(0x00);    
    
    LCD_WriteReg(0x36,0x0A);    
    
    
    LCD_WriteReg(0x3A,0x55);    
    
    
    LCD_WriteReg_Only(0x2A);    
    LCD_WriteRAM(0x00);    
    LCD_WriteRAM(0x00);    
    LCD_WriteRAM(0x01);    
    LCD_WriteRAM(0x3F);    
    
    LCD_WriteReg_Only(0x2B);        
    LCD_WriteRAM(0x00);
    LCD_WriteRAM(0x00);        
    LCD_WriteRAM(0x01);
    LCD_WriteRAM(0xE0);        
    
    delay_ms(50);
    LCD_WriteReg_Only(0x29);       //开显示 
    LCD_WriteReg_Only(0x002c);     //开始写数据
    
    LCD_SetPoint(0,0,0);
}

/***********************************************************************
*函数名： LCD_WriteRAM_Prepare
*函数功能：LCD写GRAM准备
*入口参数：无
***********************************************************************/

void LCD_WriteRAM_Prepare(void)
{
    LCD->LCD_REG = 0x002c;
}

void LCD_WriteReg_Only(u16 LCD_Reg)
{
    /* Write 16-bit Index, then Write Reg */
    LCD->LCD_REG = LCD_Reg;
}	

/***********************************************************************
*函数名： LCD_WriteRAM
*函数功能：LCD写GRAM
*入口参数：RGB_Code：要写入的颜色值（RGB565）
***********************************************************************/

void LCD_WriteRAM(u16 RGB_Code)					 
{
    /* Write 16-bit GRAM Reg */
    LCD->LCD_RAM = RGB_Code;
}

/***********************************************************************
*函数名： LCD_WriteReg
*函数功能：LCD写寄存器
*入口参数：LCD_Reg：要写的寄存器 LCD_RegValue：写入该寄存器的值
***********************************************************************/

void LCD_WriteReg(u8 LCD_Reg,u16 LCD_RegValue)
{
    /* Write 16-bit Index, then Write Reg */
    LCD->LCD_REG = LCD_Reg;
    /* Write 16-bit Reg */
    LCD->LCD_RAM = LCD_RegValue;
}

/***********************************************************************
*函数名： LCD_SetPoint
*函数功能：LCD设置坐标
*入口参数：xy：坐标值，dir：方向（0~3逆时针转）（默认竖屏）
***********************************************************************/

void LCD_SetPoint(u16 x,u16 y,u8 dir)
{
	if(dir==1)
    {
        
        LCD_WriteReg_Only(0x002A);LCD_WriteRAM(y>>8);
        LCD_WriteRAM(0x00FF&y);                 //设定X坐标
        LCD_WriteRAM(0x0001);LCD_WriteRAM(0x003F);
        
        LCD_WriteReg_Only(0x002B);LCD_WriteRAM((480-x)>>8);
        LCD_WriteRAM(0x00FF&(480-x));           //设定Y坐标
        LCD_WriteRAM(0x0001);LCD_WriteRAM(0x00df);
    }
    else if(dir==0)
    {
        LCD_WriteReg_Only(0x002A);LCD_WriteRAM(x>>8);
        LCD_WriteRAM(0x00FF&x);                  //设定X坐标
        LCD_WriteRAM(0x0001);LCD_WriteRAM(0x003F);
        
        LCD_WriteReg_Only(0x002B);LCD_WriteRAM(y>>8);
        LCD_WriteRAM(0x00FF&y);                  //设定Y坐标
        LCD_WriteRAM(0x0001);LCD_WriteRAM(0x00df);
    }
    else if(dir==2)
    {
        LCD_WriteReg_Only(0x002A);LCD_WriteRAM((320-x)>>8);
        LCD_WriteRAM(0x00FF&(320-x));                  //设定X坐标
        LCD_WriteRAM(0x0001);LCD_WriteRAM(0x003F);
        
        LCD_WriteReg_Only(0x002B);LCD_WriteRAM((480-y)>>8);
        LCD_WriteRAM(0x00FF&(480-y));                  //设定Y坐标
        LCD_WriteRAM(0x0001);LCD_WriteRAM(0x00df);
    }
    else if(dir==3)
    {
        LCD_WriteReg_Only(0x002A);LCD_WriteRAM((320-y)>>8);
        LCD_WriteRAM(0x00FF&(320-y));                  //设定X坐标
        LCD_WriteRAM(0x0001);LCD_WriteRAM(0x003F);
        
        LCD_WriteReg_Only(0x002B);LCD_WriteRAM(x>>8);
        LCD_WriteRAM(0x00FF&x);                  //设定Y坐标
        LCD_WriteRAM(0x0001);LCD_WriteRAM(0x00df);
    }
}

/***********************************************************************
*函数名： LCD_SetWindows
*函数功能：LCD设置窗口
*入口参数：StartX，StartY：开始坐标值，EndX，EndY：结束坐标值，dir：方向（0~3逆时针转）（默认竖屏）
***********************************************************************/

void LCD_SetWindows(u16 StartX,u16 StartY,u16 EndX,u16 EndY)
{
    LCD_WriteReg_Only(0x2A); 
    LCD_WriteRAM( StartX>>8);
    LCD_WriteRAM( StartX&0xFF);
    LCD_WriteRAM(EndX>>8); 
    LCD_WriteRAM(EndX&0XFF);	 
    LCD_WriteReg_Only(0x2B); 
    LCD_WriteRAM( StartY>>8);
    LCD_WriteRAM( StartY&0xFF);
    LCD_WriteRAM(EndY>>8); 
    LCD_WriteRAM(EndY&0XFF);
    LCD_WriteReg_Only(0x2C); 
}

/***********************************************************************
*函数名： LCD_DrawPoint
*函数功能：LCD画点函数
*入口参数：xy：坐标值，color：点的颜色，dir：方向（0~3逆时针转）（默认竖屏）
***********************************************************************/

void LCD_DrawPoint(u16 x,u16 y,u16 color,u8 dir)
{
    LCD_SetPoint(x,y,dir);
    LCD_WriteRAM_Prepare();
    LCD_WriteRAM(color);
}

/***********************************************************************
*函数名： LCD_Clear
*函数功能：LCD清屏函数
*入口参数：color：清屏颜色
***********************************************************************/

void LCD_Clear(u16 color)
{
    u32 index=0;
    LCD_SetWindows(0,0,319,479); 
    LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
    for(index=0;index<320*480;index++)
    {
        LCD->LCD_RAM=color;
    }
}

/***********************************************************************
*函数名： LCD_PutChar
*函数功能：LCD显示ASCLL字符
*入口参数：xy：坐标，s：字符，mode：显示模式（1覆盖，0不覆盖）
*          Color：字符颜色 BColor：背景色 dir：方向（0~3逆时针转）（默认竖屏）
***********************************************************************/

void LCD_PutChar(u16 x,u16 y,u8 s,u8 mode,u16 Color,u16 BColor,u8 dir)
{
    unsigned int i,j;
    for(i=0; i<16;i++) {
		unsigned char m=Font8x16[s*16+i];
		for(j=0;j<8;j++) {
			if((m&0x80)==0x80) {
				LCD_DrawPoint(x+j,y+i,Color,dir);
            }
			else if(mode==1) {
				LCD_DrawPoint(x+j,y+i,BColor,dir);
            }
			m<<=1;
        }
    }
}

void LCD_ShowNum(u16 x,u16 y,long num,u8 numn,u8 mode,u16 Color,u16 BColor,u8 dir)
{
	u8 n=0;
    if(num<0)
    {
        num = -num;
        LCD_PutChar(x+8*n,y,'-',mode,Color,BColor,dir);
        n++;
    }
    if(num>=1000000000)
    {	 
        LCD_PutChar(x+8*n,y,(48+num/1000000000%10),mode,Color,BColor,dir);
        n++; 
    }
    else if(numn>9){LCD_PutChar(x+8*n,y,(48+num/1000000000%10),mode,Color,BColor,dir);n++;}
    if(num>=100000000)
    {	 
        LCD_PutChar(x+8*n,y,(48+num/100000000%10),mode,Color,BColor,dir);
        n++; 
    }else if(numn>8){LCD_PutChar(x+8*n,y,(48+num/1000000000%10),mode,Color,BColor,dir);n++;}
    if(num>=10000000)
    {	 
        LCD_PutChar(x+8*n,y,(48+num/10000000%10),mode,Color,BColor,dir);
        n++; 
    }else if(numn>7){LCD_PutChar(x+8*n,y,(48+num/1000000000%10),mode,Color,BColor,dir);n++;}
    if(num>=1000000)
    {	 
        LCD_PutChar(x+8*n,y,(48+num/1000000%10),mode,Color,BColor,dir);
        n++; 
    }else if(numn>6){LCD_PutChar(x+8*n,y,(48+num/1000000000%10),mode,Color,BColor,dir);n++;}
    if(num>=100000)
    {	 
        LCD_PutChar(x+8*n,y,(48+num/100000%10),mode,Color,BColor,dir);
        n++; 
    }else if(numn>5){LCD_PutChar(x+8*n,y,(48+num/1000000000%10),mode,Color,BColor,dir);n++;}
    if(num>=10000)
    {	 
        LCD_PutChar(x+8*n,y,(48+num/10000%10),mode,Color,BColor,dir);
        n++; 
    }
    else if(numn>4){LCD_PutChar(x+8*n,y,(48+num/1000000000%10),mode,Color,BColor,dir);n++;}
    if(num>=1000)
    {	 
        LCD_PutChar(x+8*n,y,(48+num/1000%10),mode,Color,BColor,dir);
        n++; 
    }
    else if(numn>3){LCD_PutChar(x+8*n,y,(48+num/1000000000%10),mode,Color,BColor,dir);n++;}
    if(num>=100)
    {	 
        LCD_PutChar(x+8*n,y,(48+num/100%10),mode,Color,BColor,dir);
        n++; 
    }
    else if(numn>2){LCD_PutChar(x+8*n,y,(48+num/1000000000%10),mode,Color,BColor,dir);n++;}
    if(num>=10)
    {	 
        LCD_PutChar(x+8*n,y,(48+num/10%10),mode,Color,BColor,dir);
        n++; 
    }
    else if(numn>1){LCD_PutChar(x+8*n,y,(48+num/1000000000%10),mode,Color,BColor,dir);n++;}
    if(num>=0)
    {	 
        LCD_PutChar(x+8*n,y,(48+num%10),mode,Color,BColor,dir);
        n=0; 
    }
}

/***********************************************************************
*函数名： LCD_PutChar16
*函数功能：16*16汉字字符显示（自建字库版）
*入口参数：x，y：显示坐标 *data：字符串名 mode：显示模式（1覆盖，0不覆盖）
*         Color：汉字颜色 BColor：背景颜色 dir ；显示方向（0~3逆时针转）
***********************************************************************/

//void LCD_PutChar16(u16 x,u16 y,u8 *data,u8 mode,u16 Color,u16 BColor,u8 dir)
//{
// unsigned int i,j,k;
// for (k=0;k<F_NUM16;k++) 
// {                              //F_NUM16标示自建汉字库中的个数，循环查询内码
//   if ((codeGB_16[k].Index[0]==data[0])&&(codeGB_16[k].Index[1]==data[1]))
//   { 
//    	for(i=0;i<16;i++) 
//    {
//		  unsigned short m=codeGB_16[k].Msk[2*i];
//		  for(j=0;j<8;j++) 
//      {
//			   if((m&0x80)==0x80) 
//          {
//			    	LCD_DrawPoint(x+j,y+i,Color,dir);
//				  }
//			else if(mode==1) 
//          {
//				    LCD_DrawPoint(x+j,y+i,BColor,dir);
//				  }
//			  m<<=1;
//			}
//		 m=codeGB_16[k].Msk[2*i+1];
//     for(j=0;j<8;j++) 
//      {
//			   if((m&0x80)==0x80) 
//          {
//			    	LCD_DrawPoint(x+j+8,y+i,Color,dir);
//				  }
//			else if(mode==1) 
//          {
//				    LCD_DrawPoint(x+j+8,y+i,BColor,dir);
//				  }
//			  m<<=1;
//			}			
//		 }
//	 }  
// }
//}


/***********************************************************************
*函数名： LCD_PutChar16
*函数功能：16*16汉字字符显示（文件字库版）
*入口参数：x，y：显示坐标 *data：字符串名 mode：显示模式（1覆盖，0不覆盖）
*         Color：汉字颜色 BColor：背景颜色 dir ；显示方向（0~3逆时针转）
***********************************************************************/

void LCD_PutChar16(u16 x,u16 y,u8 *data,u8 mode,u16 Color,u16 BColor,u8 dir)
{
    unsigned int i,j,b=1,index;
	u8 tmp_char;
	u8 HzLib[32];
	for(b=0;b<1;b++)
	{
        index=(94*(data[b*2] - 0xa1)+(data[b*2+1] - 0xa1));	    //计算区位	
        W25QXX_Read(HzLib,index*32+1538,32);   //读取flash
    	for(i=0;i<16;i++) 
        {
            for(j=0;j<8;j++) 
            {
                tmp_char=HzLib[i*2];
                //W25QXX_Read(&tmp_char,index*32+i*2+1538,1);   //读取flash
                if ( (tmp_char << j) & 0x80)
                {
			    	LCD_DrawPoint(x+j,y+i,Color,dir);
                }
                else if(mode==1) 
                {
				    LCD_DrawPoint(x+j,y+i,BColor,dir);
                }
			}
            for(j=0;j<8;j++) 
            {
				tmp_char=HzLib[i*2+1];
				//W25QXX_Read(&tmp_char,index*32+i*2+1+1538,1);   //读取flash
                if ( (tmp_char << j) & 0x80)
                {
			    	LCD_DrawPoint(x+j+8,y+i,Color,dir);
                }
                else if(mode==1) 
                {
				    LCD_DrawPoint(x+j+8,y+i,BColor,dir);
                }
			}		
        }
    }  
}

void LCD_PutString24(u16 x,u16 y,u8 *s,u8 mode,u16 Color,u16 BColor,u8 dir)
{ 
	unsigned int i,j,b;
	u32 index;
	u8 HzLib[72];
	while(*s) 
    {
        if(((dir==0) | (dir==2)) & (x>(WIDTH-24))) {y=y+24,x=0;if(y>(HEIGHT-24))break;}
        if(((dir==1) | (dir==3)) & (x>(HEIGHT-24))) {y=y+24,x=0;if(y>(WIDTH-24))break;}
        
        for(b=0;b<1;b++)
        {
            index=(94*(s[b*2] - 0xa1)+(s[b*2+1] - 0xa1));	    //计算区位	
            W25QXX_Read(HzLib,index*72+1536+1+282752+1+1,72);   //读取flash
            for(i=0;i<24;i++) 
            {
                unsigned short m=HzLib[3*i];
                for(j=0;j<8;j++) 
                {
                    if((m&0x80)==0x80) 
                    {
                        LCD_DrawPoint(x+j,y+i,Color,dir);
                    }
                    else if(mode==1) 
                    {
                        LCD_DrawPoint(x+j,y+i,BColor,dir);
                    }
                    m<<=1;
                }
                m=HzLib[3*i+1];
                for(j=0;j<8;j++) 
                {
                    if((m&0x80)==0x80) 
                    {
                        LCD_DrawPoint(x+j+8,y+i,Color,dir);
                    }
                    else if(mode==1) 
                    {
                        LCD_DrawPoint(x+j+8,y+i,BColor,dir);
                    }
                    m<<=1;
                }	
                m=HzLib[3*i+2];
                for(j=0;j<8;j++) 
                {
                    if((m&0x80)==0x80) 
                    {
                        LCD_DrawPoint(x+j+16,y+i,Color,dir);
                    }
                    else if(mode==1) 
                    {
                        LCD_DrawPoint(x+j+16,y+i,BColor,dir);
                    }
                    m<<=1;
                }		
            }
        }  
        s+=2;
        x+=24;
    }
}

void LCD_PutString32(u16 x,u16 y,u8 *s,u8 mode,u16 Color,u16 BColor,u8 dir)
{ 
	unsigned int i,j,b;
	u32 index;
	u8 HzLib[128];
	while(*s) 
    {
        if(((dir==0) | (dir==2)) & (x>(WIDTH-32))) {y=y+32,x=0;}
        if(((dir==1) | (dir==3)) & (x>(HEIGHT-32))) {y=y+32,x=0;}
        
        for(b=0;b<1;b++)
        {
            index=(94*(s[b*2] - 0xa1)+(s[b*2+1] - 0xa1));	    //计算区位	
            W25QXX_Read(HzLib,index*128+1538+282752+1,128);   //读取flash
            for(i=0;i<32;i++) 
            {
                unsigned short m=HzLib[4*i];
                for(j=0;j<8;j++) 
                {
                    if((m&0x80)==0x80) 
                    {
                        LCD_DrawPoint(x+j,y+i,Color,dir);
                    }
                    else if(mode==1) 
                    {
                        LCD_DrawPoint(x+j,y+i,BColor,dir);
                    }
                    m<<=1;
                }
                m=HzLib[4*i+1];
                for(j=0;j<8;j++) 
                {
                    if((m&0x80)==0x80) 
                    {
                        LCD_DrawPoint(x+j+8,y+i,Color,dir);
                    }
                    else if(mode==1) 
                    {
                        LCD_DrawPoint(x+j+8,y+i,BColor,dir);
                    }
                    m<<=1;
                }	
                m=HzLib[4*i+2];
                for(j=0;j<8;j++) 
                {
                    if((m&0x80)==0x80) 
                    {
                        LCD_DrawPoint(x+j+16,y+i,Color,dir);
                    }
                    else if(mode==1) 
                    {
                        LCD_DrawPoint(x+j+16,y+i,BColor,dir);
                    }
                    m<<=1;
                }
                m=HzLib[4*i+3];
                for(j=0;j<8;j++) 
                {
                    if((m&0x80)==0x80) 
                    {
                        LCD_DrawPoint(x+j+24,y+i,Color,dir);
                    }
                    else if(mode==1) 
                    {
                        LCD_DrawPoint(x+j+24,y+i,BColor,dir);
                    }
                    m<<=1;
                }			
            }
        }  
        
        s+=2;
        x+=32;
    }
}
/***********************************************************************
*函数名： LCD_PutString
*函数功能：LCD显示字符串（汉字及ASCLL码）
*入口参数：x，y：显示坐标 *s：字符串名 mode：显示模式（1覆盖，0不覆盖）
*         fColor：汉字颜色 bColor：背景颜色 dir ；显示方向（0~3逆时针转）
***********************************************************************/


void LCD_PutString(u16 x, u16 y, char *s,u8 mode, unsigned int fColor, unsigned int bColor,u8 dir)
{
	while(*s) {
		if( *s < 0x80) 
        {
			if(((dir==0) | (dir==2)) & (x>(WIDTH-8))) {y=y+16,x=0;}
			if(((dir==1) | (dir==3)) & (x>(HEIGHT-8))) {y=y+16,x=0;}
			LCD_PutChar(x,y,*s,mode,fColor,bColor,dir);
			s++;
			x+=8;
        }
		else
        {
			if(((dir==0) | (dir==2)) & (x>(WIDTH-16))) {y=y+16,x=0;}
			if(((dir==1) | (dir==3)) & (x>(HEIGHT-16))) {y=y+16,x=0;}
			LCD_PutChar16(x,y,(unsigned char*)s,mode,fColor,bColor,dir);
			s+=2;
			x+=16;
        }
    }
}

/***********************************************************************
*函数名： LCD_showphoto
*函数功能：LCD显示图片
*入口参数：x，y：显示坐标 length，height：图片长宽 *picture：图片数组名
*         dir ；显示方向（0~3逆时针转）
***********************************************************************/

void LCD_showphoto(u16 x,u16 y,u16 length,u16 height,const u8 *picture)
{
    unsigned long i;
    LCD_SetWindows(x,y,length+x-1,height+y-1);//320x240
    LCD_WriteRAM_Prepare();
    for(i=0;i<length*height;i++)
    {
        LCD_WriteRAM((picture[i*2+1]<<8)+picture[i*2]);	 				
    }
}

void LCD_DrawLine(u16 x0,u16 y0,u16 x1,u16 y1,u16 color,u8 dir)
{
    u16 x,y;
 	u16 dx;// = abs(x1 - x0);
 	u16 dy;// = abs(y1 - y0);
    
	if(y0==y1)
	{
		if(x0<=x1)
		{
			x=x0;
		}
		else
		{
			x=x1;
			x1=x0;
		}
  		while(x <= x1)
  		{
   			LCD_DrawPoint(x,y0,color,dir);
   			x++;
  		}
  		return;
	}
	else if(y0>y1)
	{
		dy=y0-y1;
	}
	else
	{
		dy=y1-y0;
	}
    
 	if(x0==x1)
	{
		if(y0<=y1)
		{
			y=y0;
		}
		else
		{
			y=y1;
			y1=y0;
		}
  		while(y <= y1)
  		{
   			LCD_DrawPoint(x0,y,color,dir);
   			y++;
  		}
  		return;
	}
	else if(x0 > x1)
 	{
		dx=x0-x1;
  		x = x1;
  		x1 = x0;
  		y = y1;
  		y1 = y0;
 	}
 	else
 	{
		dx=x1-x0;
  		x = x0;
  		y = y0;
 	}
    
 	if(dx == dy)
 	{
  		while(x <= x1)
  		{
            
   			x++;
			if(y>y1)
			{
				y--;
			}
			else
			{
   				y++;
			}
   			LCD_DrawPoint(x,y,color,dir);
  		}
 	}
 	else
 	{
 		LCD_DrawPoint(x, y, color,dir);
  		if(y < y1)
  		{
   			if(dx > dy)
   			{
    			s16 p = dy * 2 - dx;
    			s16 twoDy = 2 * dy;
    			s16 twoDyMinusDx = 2 * (dy - dx);
    			while(x < x1)
    			{
     				x++;
     				if(p < 0)
     				{
      					p += twoDy;
     				}
     				else
     				{
      					y++;
      					p += twoDyMinusDx;
     				}
     				LCD_DrawPoint(x, y,color,dir);
    			}
   			}
   			else
   			{
    			s16 p = dx * 2 - dy;
    			s16 twoDx = 2 * dx;
    			s16 twoDxMinusDy = 2 * (dx - dy);
    			while(y < y1)
    			{
     				y++;
     				if(p < 0)
     				{
      					p += twoDx;
     				}
     				else
     				{
      					x++;
      					p+= twoDxMinusDy;
     				}
     				LCD_DrawPoint(x, y, color,dir);
    			}
   			}
  		}
  		else
  		{
   			if(dx > dy)
   			{
    			s16 p = dy * 2 - dx;
    			s16 twoDy = 2 * dy;
	    		s16 twoDyMinusDx = 2 * (dy - dx);
    			while(x < x1)
    			{
     				x++;
     				if(p < 0)
	     			{
    	  				p += twoDy;
     				}
     				else
     				{
      					y--;
	      				p += twoDyMinusDx;
    	 			}
     				LCD_DrawPoint(x, y,color,dir);
    			}
   			}
	   		else
   			{
    			s16 p = dx * 2 - dy;
    			s16 twoDx = 2 * dx;
	    		s16 twoDxMinusDy = 2 * (dx - dy);
    			while(y1 < y)
    			{
     				y--;
     				if(p < 0)
	     			{
    	  				p += twoDx;
     				}
     				else
     				{
      					x++;
	      				p+= twoDxMinusDy;
    	 			}
     				LCD_DrawPoint(x, y,color,dir);
    			}
   			}
  		}
 	}
}

/****************************************************************************
* 名    称：void GUI_Circle(u16 cx,u16 cy,u16 r,u16 color,u8 fill)
* 功    能：在指定座标画圆，可填充
* 入口参数：
* 出口参数：
* 说    明：
* 调用方法：
****************************************************************************/
void LCD_DrawCircle(u16 cx,u16 cy,u16 r,u16 color,u8 fill,u8 dir)
{
	u16 x,y;
	s16 delta,tmp;
	x=0;
	y=r;
	delta=3-(r<<1);
    
	while(y>x)
	{
		if(fill)
		{
			LCD_DrawLine(cx+x,cy+y,cx-x,cy+y,color,dir);
			LCD_DrawLine(cx+x,cy-y,cx-x,cy-y,color,dir);
			LCD_DrawLine(cx+y,cy+x,cx-y,cy+x,color,dir);
			LCD_DrawLine(cx+y,cy-x,cx-y,cy-x,color,dir);
		}
		else
		{
			LCD_DrawPoint(cx+x,cy+y,color,dir);
			LCD_DrawPoint(cx-x,cy+y,color,dir);
			LCD_DrawPoint(cx+x,cy-y,color,dir);
			LCD_DrawPoint(cx-x,cy-y,color,dir);
			LCD_DrawPoint(cx+y,cy+x,color,dir);
			LCD_DrawPoint(cx-y,cy+x,color,dir);
			LCD_DrawPoint(cx+y,cy-x,color,dir);
			LCD_DrawPoint(cx-y,cy-x,color,dir);
		}
		x++;
		if(delta>=0)
		{
			y--;
			tmp=(x<<2);
			tmp-=(y<<2);
			delta+=(tmp+10);
		}
		else
		{
			delta+=((x<<2)+6);
		}
	}
}

/****************************************************************************
* 名    称：void GUI_Rectangle(u16 x0, u16 y0, u16 x1, u16 y1,u16 color,u8 fill)
* 功    能：在指定区域画矩形，可填充颜色
* 入口参数：
* 出口参数：
* 说    明：
* 调用方法：
****************************************************************************/
void LCD_DrawRectangle(u16 x0, u16 y0, u16 x1, u16 y1,u16 color,u8 fill,u8 dir)
{
	if(fill)
	{
		u16 i;
		if(x0>x1)
		{
			i=x1;
			x1=x0;
		}
		else
		{
			i=x0;
		}
		for(;i<=x1;i++)
		{
			LCD_DrawLine(i,y0,i,y1,color,dir);
		}
		return;
	}
	LCD_DrawLine(x0,y0,x0,y1,color,dir);
	LCD_DrawLine(x0,y1,x1,y1,color,dir);
	LCD_DrawLine(x1,y1,x1,y0,color,dir);
	LCD_DrawLine(x1,y0,x0,y0,color,dir);
}

