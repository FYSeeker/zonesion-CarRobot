/*********************************************************************************************
* 文件：sample.c
* 作者：
说明：
*   传感器控制参数设置：  A0表示环境光照度数据;
*                        A1表示环境温度数据;
*                        A2表示环境湿度数据;
*                        V0表示主动上报时间间隔;
*                        D0(Bit0/Bit1)表示上报状态，默认设置D0=3;
* 修改：
* 注释：
*********************************************************************************************/
#include "sample.h"
#include "zxbee.h"
#include "pcf8563.h"
#include "w25qxx.h"
#include "bh1750.h"
#include "htu21d.h"
#include "led.h"
#include "infrared.h"
#include "rtc.h"
#include "config.h"
#include "delay.h"
#include "GPS.h"
#include "user.h"
#include "ble.h"

extern int package_data(char *buf);

/*温度传感器参数*/
extern float htu21dValue_t;
extern float htu21dValue_h;

/*********************************************************************************************
传感器控制参数
*********************************************************************************************/
u8 D0 = 0x1f;                                                   //主动上报开关，默认全开
u16 V0 = 5;                                                     //主动上报时间间隔，单位秒,0不主动上报
float A0, A1, A2;                                               //光照度、温度、湿度
RTC_TimeTypeDef RTC_TimeStruct;                                 //时间
RTC_DateTypeDef RTC_DateStruct;                                 //日期


/*********************************************************************************************
* 名称：sensor_init()
* 功能：
* 参数：
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void sensor_init(void)
{
    bh1750_init();                                              //光照度传感器
    gps_init();                                                 //gps初始化
    My_RTC_Init();                                              //初始化RTC
    RTC_Set_WakeUp(RTC_WakeUpClock_CK_SPRE_16bits,0);	        //配置WAKE UP中断,1秒中断一次
} 


/*********************************************************************************************
* 名称：sensor_poll()
* 功能：轮询传感器，并主动上报传感器数据
* 参数：无
* 返回：
* 修改：
* 注释：
*********************************************************************************************/
void sensor_poll(void)
{
    char buf[64];
    static int t = 0;
    
    RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct);
    RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);  
    
    A0 = bh1750_get();                                            //读取光照度值
    A1 = htu21dValue_t;                                           //读取温度
    A2 = htu21dValue_h;                                           //读取湿度
    
    if (V0 != 0) 
    {
        if(t >= (V0*10)) 
        {
            t = 0;
            if(D0)
            {
                if(connect_mode==1)                                          //GSM模式
                {
                    zxbeeBegin();
                    sprintf(buf, "%.1f", A0);
                    zxbeeAdd("A0", buf);
                    sprintf(buf, "%.1f", A1);
                    zxbeeAdd("A1", buf);
                    sprintf(buf, "%.1f", A2);
                    zxbeeAdd("A2", buf);
                    char *p = zxbeeEnd();
                    if (p != NULL) 
                    {
                        package_data(p);
                    }
                }
                else                                                //蓝牙模式
                {
                    zxbeeBegin();
                    sprintf(buf, "%.1f", A0);
                    zxbeeAdd("A0", buf);
                    sprintf(buf, "%.1f", A1);
                    zxbeeAdd("A1", buf);
                    sprintf(buf, "%.1f", A2);
                    zxbeeAdd("A2", buf);
                    char *p = zxbeeEnd();
                    if (p != NULL) 
                    {
                        ble_write(p);                               //通过串口蓝牙将打包的数据发送控制软件
                    }
                }
            }
        }
        else 
            t++;
    }
}




/*********************************************************************************************
* 名称：sample_z_process_command_call()
* 功能：处理上层应用发过来的指令
* 参数：ptag: 指令标识 D0，D1， A0 ...
*       pval: 指令值， “？”表示读取，
*       obuf: 指令处理结果存放地址
* 返回：>0指令处理结果返回数据长度，0：没有返回数据，<0：不支持指令。
* 修改：
* 注释：
*********************************************************************************************/
int sample_z_process_command_call(char* ptag, char* pval)
{
    char buf[16]; 
    
    if (memcmp(ptag, "MAC", 3) == 0) {                          //读取MAC
        strcpy(RADIO_MAC, pval);
    }
    else if (memcmp(ptag, "A0", 2) == 0) {                      //查询A0的值
        if (pval[0] == '?') {
            sprintf(buf, "%f", A0);
            zxbeeAdd("A0", buf);
        }
    }
    else if (memcmp(ptag, "A1", 2) == 0) {                      //查询A1的值
        if (pval[0] == '?') {
            sprintf(buf, "%f", A1);
            zxbeeAdd("A1", buf);
        }
    }
    else if (memcmp(ptag, "A2", 2) == 0) {                      //查询A2的值
        if (pval[0] == '?') {
            sprintf(buf, "%.1f", A2);
            zxbeeAdd("A2", buf);
        }
    }
    else if(memcmp(ptag, "D0", 2) == 0)
    {
        if(pval[0] == '?')                                      //查询D0的值
        {           
            sprintf(buf, "%d", D0);
            zxbeeAdd("D0", buf);                                      
        }
        else                                                    //修改D0的值
        {            
            int value = atoi(pval);
            if (value >= 0) 
            {
                D0 = value;
            }
        }
    }
    else if(memcmp(ptag, "D1", 2) == 0)
    {
        if(pval[0] == '?')                                      //查询D1的值
        {           
            sprintf(buf, "%d", D1);
            zxbeeAdd("D1", buf);                                      
        }
        else                                                    //修改D1的值
        {            
            int value = atoi(pval);
            if (value >= 0) 
            {
                D1 = value;
            }
        }
    }
    
    return 0;
}