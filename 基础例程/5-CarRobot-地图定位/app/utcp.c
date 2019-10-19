/*********************************************************************************************
* 文件：fallalg.c
* 作者：zonesion 2016.12.2
* 说明：跌落程序
* 修改：Meixin 2017.05.31 增加了注释
* 注释：
*********************************************************************************************/

/*********************************************************************************************
* 头文件
*********************************************************************************************/
#include <contiki.h>
#include <stdio.h>
#include <string.h>
#include <sys/stimer.h>
#include "gsm.h"
#include "zxbee.h"
//#include "zxbee.h"


PROCESS(utcp, "utcp");
int gsm_tcp_0_send(int len);
#define app_id  "2093722535"
#define app_key "qye7RhIYNwXAccrsY0UlwYkSPAd71cOQ"

/*********************************************************************************************
* 定义
*********************************************************************************************/
static struct etimer et;                                        //etimer定时器
struct stimer tcp_0_timer;                                      //stimer定时器
static int tcp_0_status = 0;                                    //tcp状态标志
static int request_send_cced_flag = 0;                          //请求页发送函数
static int tcp_0_send_len = 0;                                  //发送数据长度
static char tcp_send_buf[256];                                  //存储数据数组
static char cced_buf[24*7];                                     //基站信息缓存
static void package_decode(char *buf, int len);                 //数据包解码函数

/*********************************************************************************************
* 名称：void _rs_gsm_query_cced(char *rsp)
* 功能：GSM查询
* 参数：
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void _rs_gsm_query_cced(char *rsp)
{
  if (rsp == NULL) {
    return;
  };
  if (memcmp(rsp, "OK", 2) == 0) {                              //OK指令匹配
    printf(" cced %s\r\n", cced_buf);
    if (strlen(cced_buf) > 0) {
      if (tcp_0_send_len == 0) {
        sprintf(tcp_send_buf, "{\"method\":\"sensor\",\"data\":\"{A2=%s}\"}", cced_buf);
        gsm_tcp_0_send(strlen(tcp_send_buf));
      } else {
        request_send_cced_flag = 1;
      }
    }
  }
  if (memcmp(rsp, "+CCED:", 6) == 0) {                          //“+CCED”指令
    //+CCED: 460,0,712c,df6a,536,56,34,0,0,0,0,0,0
    if (memcmp(rsp, "+CCED: 0,0,0,0,0,0,0,0,0,0,0,0,0", 32) == 0) return;
    char *p1 = &rsp[7];
    char *e1 = strchr(p1, ',');
    char *e2 = strchr(e1+1, ',');
    char *e3 = strchr(e2+1, ',');
    char *e4 = strchr(e3+1, ',');
    *e4 = 0;
    char *e5 = strchr(e4+1, ',');
    char *e6 = strchr(e5+1, ',');
    char *e7 = strchr(e6+1, ',');
    *e7 = 0;
 
    char dlen = strlen(cced_buf);
    if (dlen > 0) {
      cced_buf[dlen++] = ';';
    }
    strcpy(&cced_buf[dlen], p1);
    dlen = strlen(cced_buf);
    cced_buf[dlen++] = ',';
    strcpy(&cced_buf[dlen], e6+1);
  }
}
/*********************************************************************************************
* 名称：void gsm_query_cced(void)
* 功能：GSM查询
* 参数：
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void gsm_query_cced(void)
{
  cced_buf[0] = 0;
  gsm_request_at("at+cced=0,15\r\n", 3000, _rs_gsm_query_cced);
}

/*********************************************************************************************
* 名称：void _rs_gsm_tcp_0_status(char *rsp)
* 功能：GSM TCP状态
* 参数：
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void _rs_gsm_tcp_0_status(char *rsp)
{
  if (rsp == NULL) return;
  if (strcmp(rsp, "+ZIPSTATUS:ESTABLISHED") == 0) {
    tcp_0_status = 2;
  }
  if (strcmp(rsp, "+ZIPSTATUS:DISCONNECTED") == 0) {
    tcp_0_status = 0;
  }
}
/*********************************************************************************************
* 名称：void gsm_tcp_0_status(void)
* 功能：GSM TCP状态
* 参数：
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void gsm_tcp_0_status(void)
{
  gsm_request_at("at+zipstatus=0\r\n", 3000, _rs_gsm_tcp_0_status);
}
/*********************************************************************************************
* 名称：void _rs_gsm_tcp_0_open(char *rsp)
* 功能：GSM TCP打开
* 参数：
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void _rs_gsm_tcp_0_open(char *rsp)
{
  if (rsp == NULL) return;
  if (strcmp(rsp, "+ZIPSETUP:CONNECTED") == 0) {
    gsm_tcp_0_status();
  }
}


/*********************************************************************************************
* 名称：void gsm_tcp_0_open(void)
* 功能：GSM TCP打开
* 参数：
* 返回：无
* 修改：120.24.67.222
* 注释：
*********************************************************************************************/
void gsm_tcp_0_open(void)
{
  
  gsm_request_at("at+zipclose=0\r\n", 3000, NULL);
  gsm_request_at("at+ziptimeout=15,30\r\n", 3000, NULL);
  gsm_request_at("at+zipsetup=0,120.24.67.222,28082\r\n", 30000, _rs_gsm_tcp_0_open);
}
/*********************************************************************************************
* 名称：void gsm_tcp_0_close(void)
* 功能：GSM TCP关闭
* 参数：
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void gsm_tcp_0_close(void)
{
   gsm_request_at("at+zipclose=0\r\n", 3000, NULL);
}
/*********************************************************************************************
* 名称：void _rs_gsm_tcp_0_send(char *rsp)
* 功能：GSM TCP发送
* 参数：
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/

void _rs_gsm_tcp_0_send(char *rsp)
{
  if (rsp == NULL) {
    tcp_0_send_len = 0;
    return;
  }
  if (strcmp(rsp, "ERROR") == 0) {
    tcp_0_send_len = 0;
    return;
  }
  if (strcmp(rsp, ">") == 0) {
    printf("tcp <<< %s\r\n", tcp_send_buf);
    gsm_tcp_write(tcp_send_buf, tcp_0_send_len);
    tcp_0_send_len = 0;
    stimer_set(&tcp_0_timer, 60);
  }
}

/*********************************************************************************************
* 名称：int gsm_tcp_0_send(int len)
* 功能：GSM TCP发送数据描述
* 参数：
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
int gsm_tcp_0_send(int len)
{
  char buf[32];
  if (tcp_0_send_len == 0 && (gsm_info.phone_status == PHONE_ST_IDLE)) {
    tcp_0_send_len = len;
    sprintf(buf, "at+zipsend=0,%d\r", len);
    gsm_request_at(buf, 30000, _rs_gsm_tcp_0_send);
    return len;
  }
  return -1;
}

/*********************************************************************************************
* 名称：void gsm_tcp_0_recv(char *buf, int len)
* 功能：GSM TCP接收
* 参数：
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void gsm_tcp_0_recv(char *buf, int len)
{
  printf(" tcp >> %s\r\n", buf);
  //for (int i=0; i<len; i++) printf(" %02X", buf[i]);
  //printf("\r\n");
  package_decode(buf, len);
}


/*********************************************************************************************
* 名称：int tag_process_b(void)
* 功能：标记过程_b
* 参数：
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
int tag_process_b(void)
{
  if (tcp_0_send_len == 0) {
     //{"method":"sensor","data":"adc"}
    sprintf(tcp_send_buf, "{\"method\":\"sensor\",\"data\":\"{");
    //tcp_send_buf[0] = '\0';
    return 0;
  }
  printf("w: tcp 0 send buf not empty!\r\n");
  return -1;
}

/*********************************************************************************************
* 名称：void tag_process_e(void)
* 功能：标记过程_e
* 参数：
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void tag_process_e(void)
{
  int len = strlen(tcp_send_buf);
  tcp_send_buf[len++] = '}'; 
  tcp_send_buf[len++] = '\"'; //replace '&'
  tcp_send_buf[len++] = '}';
  tcp_send_buf[len] = 0;
  if (len > 29) {
    gsm_tcp_0_send(strlen(tcp_send_buf));                        //发送数据信息
  }
}
/*********************************************************************************************
* 名称：void package_decode(char *buf, int len)
* 功能：包解码
* 参数：
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void package_decode(char *buf, int len)
{
  if (buf[len-1] != 0) {
    printf("error: tcp 0 package format\r\n");
    return;
  }
  len --;
  //printf(buf);
  //printf("\r\n");
  /* {"method":"control","data":"adc"} */
  if (strstr(buf, "method") && strstr(buf, "control")) {
    char *pdat = strstr(buf, "data");
    if (pdat) {
      pdat = strstr(pdat, ":\"{");
      pdat += 3;
      buf[len - 3] = 0;
      printf("data : %s\r\n", pdat);
      
        if (tag_process_b() < 0) return;
 
        char *p = pdat;
        while (p != NULL) {
          char *e = strchr(p, ',');
          char *np = NULL;
          if (e != NULL) {
            *e = 0;
            np = e + 1;
          } 
          char *v = strchr(p, '=');
          if (v != NULL) {
            *v = '\0';
            v = v + 1;
            int ret;
            ret = z_process_command_call(p, v);
            //tag_process(p, v);
            if ((ret > 0) && (np > 0)) {
              int buf_lens = strlen(tcp_send_buf);
              tcp_send_buf[buf_lens] = ',';
              tcp_send_buf[buf_lens+1] = 0;
        }
          }
          p = np; 
        }
        if (tcp_send_buf[28] != 0)
          tag_process_e();
    }
  }
}
/*********************************************************************************************
* 名称：void package_decode(char *buf, int len)
* 功能：认证包
* 参数：
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
int package_auth(void)
{
#if 0
  tcp_send_buf[0] = 'T';
  memcpy(&tcp_send_buf[1], gsm_info.imei, 15);
  tcp_send_buf[16] = '\n';
  return  17;
#else
  //{"method":"authenticate","uid":"12345","key":"3Wl6cI8TNi8fMkhCXkZzXzCTsst4LgqY","addr":"adfbc"}
  sprintf(tcp_send_buf,
     "{\"method\":\"authenticate\",\"uid\":\"%s\",\"key\":\"%s\",\"addr\":\"%s\",\"version\":\"0.1.0\", \"autodb\":false}",
     app_id, app_key, gsm_info.imei);
  return strlen(tcp_send_buf);
#endif
}
/*********************************************************************************************
* 名称：int package_heartbeat(void)
* 功能：心跳包
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
int package_heartbeat(void)
{
#if 0
  tcp_send_buf[0] = '\n';
  return 1;
#else
  static unsigned int seq = 0;
  sprintf(tcp_send_buf, "{\"method\":\"echo\", \"timestamp\":(%u, \"seq\":%u)}", 
          clock_time(), seq++);
  return strlen(tcp_send_buf);
#endif
}

/*********************************************************************************************
* 名称：int package_data(char *buf)
* 功能：数据包
* 参数：
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
int package_data(char *buf)
{
  if (tcp_0_status == 2 && tcp_0_send_len == 0) 
  {
    sprintf(tcp_send_buf, "{\"method\":\"sensor\",\"data\":\"%s\"}", buf);
    gsm_tcp_0_send(strlen(tcp_send_buf));
    return 1;
  } 
  else 
  {
    printf("tcp can't send %s\r\n", buf);
    printf("tcp not connect or busy\r\n");
    return 0;
  }
}
/*********************************************************************************************
* 名称：PROCESS_THREAD(utcp, ev, data)
* 功能: UTCP进程线
* 参数：
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
PROCESS_THREAD(utcp, ev, data)
{ 
  PROCESS_BEGIN();
  
  gsm_tcp_recv_register(0, gsm_tcp_0_recv);                     //绑定处理函数
  while (1) {
    static unsigned int tick = 0;
    static int last_tcp_0_status = 0;
    tick ++;
    if (gsm_info.ppp_status == 2) {                             //链接状态
      if (tcp_0_status == 0) {
        gsm_tcp_0_open();                                       //打开tcp
        tcp_0_status = 1;
        stimer_set(&tcp_0_timer, 60);                           //定时器设置
      }
      if (tcp_0_status == 1 && stimer_expired(&tcp_0_timer)) {
        tcp_0_status = 0;
      }
      if (tcp_0_status != 0) { // 
        
        if (tick % 30 == 1) {                                   // check tcp 0 connect;
          gsm_tcp_0_status();
        }
      }
      if (tcp_0_status == 2) {
        if (last_tcp_0_status != 2) {                           //第1次
          gsm_tcp_0_send(package_auth());
        }
        if (request_send_cced_flag != 0) {
          if (tcp_0_send_len == 0) {
            sprintf(tcp_send_buf, "{\"method\":\"sensor\",\"data\":\"{A2=%s}\"}", cced_buf);
            gsm_tcp_0_send(strlen(tcp_send_buf));
            request_send_cced_flag = 0;
          }
        }
        if (tcp_0_send_len==0 && stimer_expired(&tcp_0_timer)) {
          gsm_tcp_0_send(package_heartbeat());                  //发送数据描述信息
        }
      }
      last_tcp_0_status = tcp_0_status;
    }
    
    etimer_set(&et, CLOCK_SECOND);                              //定时器设置
    PROCESS_YIELD();
  }
  
  PROCESS_END();
}
