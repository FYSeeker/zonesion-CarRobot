#include "flamanage.h"
#include "config.h"
#include <contiki.h>
#include "w25qxx.h"
#include "user.h"



void config_save_mode()
{
    FlaVPageWrite(5, 0, &connect_mode, 1);
}


void config_read_mode()
{
    FlaVPageRead(5, 0, &connect_mode, 1);
}


void config_init(void)
{
    FlaManageInit();
    int flag = 0;
    FlaVPageRead(1, 0, &flag, 4);
    if (flag != 0x5a5a5a5c) 
    {
        FlaFormat();
        flag = 0x5a5a5a5c;
        FlaVPageWrite(1, 0, &flag, 4);
        config_save_mode();
    } 
    else 
    {
        config_read_mode();
    }
}



PROCESS(config, "config");

/*********************************************************************************************
* 名称：PROCESS_THREAD(config, ev, data)
* 功能：定义进程
* 参数：
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
PROCESS_THREAD(config, ev, data)
{
    PROCESS_BEGIN();                                              //开始进程
    W25QXX_Init();
    config_init();                                                //配置初始化
    PROCESS_END();                                                //结束进程
}