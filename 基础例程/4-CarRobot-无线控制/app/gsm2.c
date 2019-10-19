#include <contiki.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gsm.h"
#include "sys/ctimer.h"
#include "mg2639.h"
#include "user.h"

/*********************************************************************************************
* 宏定义
*********************************************************************************************/
#define _RESPONSE_BUF_SIZE 256
PROCESS(gsm, "gsm");

gsm_info_t gsm_info;

static int gsm_response_mode = 0;
static char  _response_buf[_RESPONSE_BUF_SIZE];
static int _response_offset = 0;

//int gsm_phone_status = PHONE_ST_IDLE;
//at指令结构体
struct _at_request{
    void *req;
    void (*fun)(char * response);
    unsigned int timeout_tm;
    struct _at_request *next;
};

struct _at_request *current_at = NULL;
struct _at_request * list_at = NULL;

/*********************************************************************************************
* 名称：_request_at_1(char *at)
* 功能：请求 1
* 参数：*at --  
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void _request_at_1(char *at)
{
    _request_at_3(at, 1000, NULL);
}

/*********************************************************************************************
* 名称：int _request_at_3(char *at, unsigned int to, void (*fun)(char*))
* 功能：添加at指令到指令等待列表
* 参数：*at --     to --   
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
int _request_at_3(char *at, unsigned int to, void (*fun)(char*))
{
    int len = strlen(at);                                         //请求指令长度
    char *p = malloc(len+1);                                      //申请内存
    if (p) strcpy(p, at);                                         //复制指令
    else return -1;
    struct _at_request *pa = malloc(sizeof (struct _at_request)); //申请内存
    if (pa) {
        pa->req = p;
        pa->next = NULL;
        pa->fun = fun;
        pa->timeout_tm = to;                                        //记录超时时间
        if (list_at == NULL) list_at = pa;
        else {
            struct _at_request *pl = list_at;
            while (pl->next != NULL) pl = pl->next;
            pl->next = pa; 
        }
    } else {
        free(p);                                                    //释放内存
        return -1;
    }
    return 0;
}
/*********************************************************************************************
* 名称：end_process_at(void)
* 功能：结束处理函数
* 参数：无 
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void end_process_at(void)
{
    if (current_at != NULL) {                                     //指令等待列表不为空
        free(current_at->req);
        free(current_at);
        current_at = NULL;
    }
}
////////////////////////////////////////////////////////////////////////////////
void check_sim_card(void);
/*********************************************************************************************
* 名称：static void _rp_check_gsm(char *rsp)
* 功能：静态的GSM检查函数
* 参数：*rsp -- 
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
static void _rp_check_gsm(char *rsp)                            //GSM监测
{
    if (rsp == NULL) {
        //_request_at_3("at\r\n", 3000, _rp_check_gsm);
    } else if (memcmp(rsp, "OK", 2) == 0) {
        gsm_info.gsm_status = 1;
    }
}
/*********************************************************************************************
* 名称：void check_gsm(void)
* 功能：GSM检查函数
* 参数：*rsp -- 
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void check_gsm(void)
{
    _request_at_3("at\r\n", 3000, _rp_check_gsm);
}

/*********************************************************************************************
* 名称：static void _rs_check_sim_card(char *rsp)
* 功能：静态的SIM检查函数
* 参数：*rsp -- 
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/

static void _rs_check_sim_card(char *rsp)
{
    if (rsp == NULL) {
    } 
    if (memcmp(rsp, "*TSIMINS:", 9)==0) {
        if (rsp[12] == '1') {
            gsm_info.gsm_status = 2;
            gsm_info.simcard_status = 1;
        } else {
            gsm_info.gsm_status = 3;
            gsm_info.simcard_status = 0;     
        }
    } 
}

/*********************************************************************************************
* 名称：void check_sim_card(void)
* 功能：SIM卡检查函数
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void check_sim_card(void)
{
    //_request_at_3("at+cpin?\r\n", 3000, _rs_check_sim_card);
    _request_at_3("at*tsimins?\r\n", 3000, _rs_check_sim_card);
}

/*********************************************************************************************
* 名称：static void _rs_register_network(char *rsp)
* 功能：登记入网
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
static void _rs_register_network(char *rsp)
{
    if (rsp != NULL && memcmp(rsp, "+COPS:", 6) == 0) {
        //+COPS: 0,2,"46000"
        if (strlen(rsp) > 12) {
            char *p = &rsp[12];
            char *e = strchr(p, '\"');
            if (e) {
                *e = '\0';
                strcpy(gsm_info.network, p);
            }
        }
    }
}
/*********************************************************************************************
* 名称：void gsm_register_network(void)
* 功能：GSM登记入网函数
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void gsm_register_network(void)
{
    _request_at_3("at+cops=0,2\r\n", 5000, NULL);
    _request_at_3("at+cops?\r\n", 5000, _rs_register_network);
}
/*********************************************************************************************
* 名称：void _rs_gsm_call_accept(char *res)
* 功能：GSM电话接收
* 参数：*res -- 
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void _rs_gsm_call_accept(char *res)
{
    if (res == NULL) {
        printf(" error : gsm mast not timeout whic ruan ata\r\n");
        return;
    }
    if (memcmp(res, "OK", 2) == 0) {
        gsm_info.phone_status = PHONE_ST_TALK;
    } 
    if (memcmp(res, "ERROR", 5) == 0) {
        gsm_info.phone_status = PHONE_ST_IDLE;
    }/*
    if (memcmp(res, "NO CARRIER", 10) == 0) {
    gsm_info.phone_status = PHONE_ST_IDLE;
}*/
}
/*********************************************************************************************
* 名称：void gsm_call_accept(void)
* 功能：GSM电话接收
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void gsm_call_accept(void)
{
    if (gsm_info.phone_status == PHONE_ST_RING) {
        _request_at_3("ata\r\n", 1000, _rs_gsm_call_accept);
    }
}
/*********************************************************************************************
* 名称：void _rs_gsm_call_hangup(char *rsp)
* 功能：GSM电话挂断
* 参数：*rsp -- 
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void _rs_gsm_call_hangup(char *rsp){
    gsm_info.phone_status = PHONE_ST_IDLE;
}

/*********************************************************************************************
* 名称：void gsm_call_hangup(void)
* 功能：GSM电话挂断
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void gsm_call_hangup(void)
{
    _request_at_3("ath\r\n", 1000, _rs_gsm_call_hangup);
}
void _rs_gsm_request_call(char *rsp)
{
    if (rsp == NULL) {
        printf("error : gsm must not timeout when run atd\r\n");
        return;
    }
    if (memcmp(rsp, "OK", 2) == 0) {
        gsm_info.phone_status = PHONE_ST_COUT;
    }
    if (memcmp(rsp, "ERROR", 5) == 0) {
        gsm_info.phone_status = PHONE_ST_IDLE;
    } /*
    if (memcmp(rsp, "NO CARRIER", 10) == 0) {
    gsm_info.phone_status = PHONE_ST_IDLE;
}
    if (memcmp(rsp, "BUSY", 4) == 0) {
    gsm_info.phone_status = PHONE_ST_IDLE;
}*/
}
/*********************************************************************************************
* 名称：int gsm_request_call(char *pnum)
* 功能：GSM请求呼叫
* 参数：*rsp
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
int gsm_request_call(char *pnum)
{
    char buf[32];
    sprintf(buf, "atd%s;\r\n", pnum);
    if (gsm_info.phone_status == PHONE_ST_IDLE) {
        strcpy(gsm_info.phone_number, pnum);
        _request_at_3(buf, 2000, _rs_gsm_request_call);
        return 1;
    }
    else return 0;
}
/*********************************************************************************************
* 名称：void _rs_gsm_get_current_call(char *rsp)
* 功能：GSM得到当前的电话
* 参数：*rsp
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void _rs_gsm_get_current_call(char *rsp)
{
    if (memcmp(rsp, "OK", 2) == 0) {
    }
    if (memcmp(rsp, "+CLCC", 5) == 0) {
        //+CLCC: 1,0,0,0,0,"10010",129
        if (strlen(rsp)>12) {
            if (rsp[11] == '0') {
                gsm_info.phone_status = PHONE_ST_TALK;
            }
        }
    }
}
/*********************************************************************************************
* 名称：void gsm_get_current_call(void)
* 功能：GSM得到当前的电话
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void gsm_get_current_call(void)
{
    _request_at_3("at+clcc\r\n", 1000, _rs_gsm_get_current_call);
}
/*********************************************************************************************
* 名称：void gsm_ppp_open(void)
* 功能：GSM点对点通讯打开
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/

void gsm_ppp_open(void)
{
    char *apn = "cmnet";
    char buf[32];
    if (memcmp(gsm_info.network, "46000", 5) == 0) {
        apn = "cmnet";
    }
    if (memcmp(gsm_info.network, "46001", 5) == 0) {
        apn = "3gnet";
    }
    sprintf(buf, "at+zpnum=\"%s\",\"\",\"\"\r\n", apn);
    _request_at_3(buf, 3000, NULL);
    _request_at_3("at+zpppopen\r\n", 3000, NULL);
    _request_at_3("at+zpppstatus\r\n", 3000, NULL);
}
/*********************************************************************************************
* 名称：void _rs_gsm_get_imei(char *rsp)
* 功能：GSM收到手机串号
* 参数：*rsp
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void _rs_gsm_get_imei(char *rsp)
{
    if (rsp == NULL) {
        printf("error: get imei not timeout\r\n");
        return;
    }
    int len = strlen(rsp);
    if (len == 15) {
        strcpy(gsm_info.imei, rsp);
    }
}
/*********************************************************************************************
* 名称：void _rs_gsm_get_imei(char *rsp)
* 功能：GSM收到手机串号
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void gsm_get_imei(void)
{
    _request_at_3("at+gsn\r\n", 1000, _rs_gsm_get_imei);
}
/*********************************************************************************************
* 名称：void _rs_gsm_send_message(char *rsp)
* 功能：GSM发送短信
* 参数：*rsp
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
static char short_message[72];
void _rs_gsm_send_message(char *rsp)
{
    if (rsp == NULL) {
        printf("error: send message timeout\r\n");
        return;
    }
    if (rsp[0] == '>')  {
        mg2639_write(short_message, strlen(short_message));
        char ch = 0x1A;
        mg2639_write(&ch, 1);
        printf("\r\nGSM <<< %s\r\n", short_message);
    }
}
/*********************************************************************************************
* 名称：void gsm_send_message(char *num, char *msg)
* 功能：GSM发送短信
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void gsm_send_message(char *num, char *msg)
{
    char buf[32];
    sprintf(buf, "at+cmgs=\"%s\"\r", num);
    sprintf(short_message, msg);
    _request_at_3(buf, 5000, _rs_gsm_send_message);
}

/*********************************************************************************************
* 名称：void _rs_gsm_query_creg(char *rsp)
* 功能：GSM查询
* 参数：*rsp
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void _rs_gsm_query_creg(char *rsp)
{
    /* +CREG: 2, 1, "712C", "DF6A", 0 */
    if (rsp == NULL) return;
    if (memcmp(rsp, "+CREG:", 6) == 0) {
        if (rsp[7] != '2') {
            _request_at_3("at+creg=2\r\n", 1000, NULL);
            return;
        } else {
            if (strlen(rsp) > 14) {
                char *lac = &rsp[14];
                char *p = strchr(lac, '\"');
                *p = '\0';
                char *ci = p+4;
                p = strchr(ci, '\"');
                *p = '\0';
                char *act = p + 3;
                //printf(" got lac: %s, ci: %s, act:%s\r\n", lac, ci, act);
                strcpy(gsm_info.lac, lac);
                strcpy(gsm_info.ci, ci);
                strcpy(gsm_info.act, act);
            }
        }
    }
}
/*********************************************************************************************
* 名称：void gsm_query_creg(void)
* 功能：GSM查询
* 参数：*rsp
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void gsm_query_creg(void)
{
    _request_at_3("at+creg?\r\n", 5000, _rs_gsm_query_creg);
}

/*********************************************************************************************
* 名称：int gsm_tcp_recv_register(int id, void (*fun)(char *buf, int len))
* 功能：GSM TCP接收寄存器
* 参数：*rsp
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
static void (*tcp_recv_call[5])(char *buf, int len);
int gsm_tcp_recv_register(int id, void (*fun)(char *buf, int len))
{
    if (id >= 0 && id < 5) {
        tcp_recv_call[id] = fun;
        return 0;
    } 
    return -1;
}

/*********************************************************************************************
* 名称：static void gsm_tcp_recv(int id, int len, char *buf)
* 功能：GSM TCP接收
* 参数：*rsp
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
static void gsm_tcp_recv(int id, int len, char *buf)
{
    if (id >= 0 && id < 5) {
        if (tcp_recv_call[id] != NULL) {
            tcp_recv_call[id](buf, len);
        }
    }
}
/*********************************************************************************************
* 名称：gsm_tcp_write(char *buf, int len)
* 功能：GSM TCP写入
* 参数：*buf --    len -- 
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void gsm_tcp_write(char *buf, int len)
{
    mg2639_write(buf, len);
}
/*********************************************************************************************
* 名称：void gsm_tcp_close(int id)
* 功能：GSM TCP关闭
* 参数：id
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void gsm_tcp_close(int id)
{
}
static struct ctimer gsm_timer;

/*********************************************************************************************
* 名称：static void _gsm_timer_call(void *ptr)
* 功能：GSM 定时器呼叫
* 参数：*ptr
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/

static void _gsm_timer_call(void *ptr)
{
    static int tick = 0;
    tick ++;
    if (gsm_info.gsm_status == 0) {
        if (tick % 3 == 0) {
            check_gsm();
        }
    }
    if (gsm_info.gsm_status == 1) {
        static int init = 0;
        if (init == 0) {
            _request_at_3("ate0\r\n", 1000, NULL);
            _request_at_3("at+clip=1\r\n", 1000, NULL);
            _request_at_3("at+speaker=0\r\n", 1000, NULL);
            //_request_at_3("at+tsimins?\r\n", 1000, NULL);
            //_request_at_3("at+cops=0,2\r\n", 1000, NULL);
            _request_at_3("at+zcsq=3\r\n", 1000, NULL);
            gsm_get_imei();
            _request_at_3("at+creg=2\r\n", 1000, NULL);
            //_request_at_3("at+cced=1,3\r\n", 1000, NULL);
            
            _request_at_3("at+cmgf=1\r\n", 1000, NULL);
            init = 1;
        }
        if (init == 1 && tick % 5 == 0) {
            check_sim_card();
        }
    }
    if (gsm_info.gsm_status == 2) {
        
        if (tick % 5 == 1 && strlen(gsm_info.network) == 0) {
            gsm_register_network();
        }
        if (strlen(gsm_info.network) > 0) {
            if (tick % 5 == 2 && gsm_info.ppp_status == 0) {
                gsm_ppp_open();
            }
            /*if (gsm_info.ppp_status != 0 && tick % 60 == 30) {
            _request_at_3("at+zpppstatus\r\n", 1000, NULL);
        }*/
        }
        /*if (tick % 60 == 15) {
        gsm_query_creg();
    }*/
        if (gsm_info.phone_status == PHONE_ST_COUT) {
            gsm_get_current_call();
        }
    }
    if (gsm_info.gsm_status == 3) {
        //无sim卡
        if (tick % 5 == 0) check_sim_card();
    }
    ctimer_reset(&gsm_timer);
}

/*********************************************************************************************
* 名称：void _poll_request(void)
* 功能：轮询请求
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void _poll_request(void)
{
    if (current_at == NULL) {
        current_at = list_at;
        if (current_at != NULL) {
            printf(" gsm <<< %s", (char*)current_at->req);
            if (strstr(current_at->req, "zipsend")) {
                printf("\n");
            }
            
            current_at->timeout_tm = clock_time()+ current_at->timeout_tm;
            mg2639_write(current_at->req, strlen(current_at->req));
            list_at = list_at->next;
        }
    } else {
        unsigned int tm = clock_time();
        if (((int)(tm - current_at->timeout_tm)) > 0) {
            printf(" gsm : timeout\r\n");
            if (current_at->fun != NULL) {
                current_at->fun(NULL);
            }
            end_process_at();
        } 
    }
}
/*********************************************************************************************
* 名称：void _poll_response(void)
* 功能：轮询反应
* 参数：无
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void _poll_response(void)
{
    int ch;
    ch = mg2639_get_ch();
    while (ch >= 0) {
        if (gsm_response_mode == 0) {
            if (_response_offset == 0 && ch == '>') {
                if (current_at != NULL && current_at->fun != NULL) {    
                    current_at->fun(">");
                }
            } else if (_response_offset >= 9 && memcmp(_response_buf, "+ZIPRECV:", 9) == 0) {
                _response_offset = 0;
                /* 获取tcp id */
                do {
                    ch = mg2639_get_ch();
                    if (ch >= 0) {
                        _response_buf[_response_offset++] = ch;
                    }
                } while (ch != ',');
                _response_buf[_response_offset-1] = '\0';
                int id = atoi(_response_buf);
                _response_offset = 0;
                /* 获取数据长度 */
                do {
                    ch = mg2639_get_ch();
                    if (ch >= 0) {
                        _response_buf[_response_offset++] = ch;
                    }
                } while (ch != ',');
                _response_buf[_response_offset-1] = '\0';
                int dlen = atoi(_response_buf);
                printf("gsm >>> +ZIPRECV: %d,%d\r\n", id, dlen);
                /* 读取数据 */
                do {       
                    _response_offset=0;
                    int min = dlen < _RESPONSE_BUF_SIZE ? dlen : _RESPONSE_BUF_SIZE;
                    do {
                        ch = mg2639_get_ch();
                        if (ch >= 0) {
                            _response_buf[_response_offset++] = ch;
                        }
                    } while (_response_offset < min);
                    //_response_buf[_response_offset] = 0;
                    gsm_tcp_recv(id, _response_offset, _response_buf);
                    dlen -= _response_offset;
                } while (dlen > 0);
                _response_offset = 0;
            } else
                if (ch != '\n') {
                    _response_buf[_response_offset++] = ch;
                } else {
                    if (_response_offset>0 && _response_buf[_response_offset-1] == '\r') {
                        _response_buf[_response_offset-1] = '\0';
                        _response_offset -= 1;
                        if (_response_offset > 0) {
                            printf(" gsm >>> %s\r\n", _response_buf);
                            if (current_at != NULL && current_at->fun != NULL) {
                                current_at->fun(_response_buf);
                            }
                            if ((memcmp(_response_buf, "OK", 2) == 0) 
                                ||(memcmp(_response_buf, "ERROR", 5) == 0))
                            {
                                end_process_at();
                            }
                            /* unsolite message */
                            if (memcmp(_response_buf, "+CSQ:", 5) == 0) {
                                char *p = &_response_buf[6];
                                char *e = strchr(p, ',');
                                *e = '\0';
                                int v = atoi(p);
                                if (v == 99) v = 0;
                                gsm_info.signal = v/6;
                            }
                            if (memcmp(_response_buf, "+CLIP: ", 7) == 0) {
                                //+CLIP: "13135308483",161,"","",0
                                gsm_info.phone_status = PHONE_ST_RING;
                                char *p = &_response_buf[8];
                                char *e = strchr(p, '\"');
                                *e = '\0';
                                strcpy(gsm_info.phone_number, p);
                            }
                            if (memcmp(_response_buf, "NO CARRIER", 10) == 0) {
                                gsm_info.phone_status = PHONE_ST_IDLE;
                            }
                            if (memcmp(_response_buf, "BUSY", 4) == 0) {
                                gsm_info.phone_status = PHONE_ST_IDLE;
                            }
                            if (memcmp(_response_buf, "+ZPPPOPEN:CONNECTED", 19) == 0) {
                                gsm_info.ppp_status = 1;
                            }
                            if (memcmp(_response_buf, "+ZPPPOPEN:ESTABLISHED", 21) == 0) {
                                gsm_info.ppp_status = 2;
                            }
                            if (memcmp(_response_buf, "+ZPPPSTATUS:ESTABLISHED", 23) == 0) {
                                gsm_info.ppp_status = 2;
                            }
                            if (memcmp(_response_buf, "+ZPPPSTATUS:CONNECTED", 21) == 0) {
                                gsm_info.ppp_status = 1;
                            }
                            if (memcmp(_response_buf, "+ZPPPSTATUS:DISCONNECTED", 24) == 0) {
                                gsm_info.ppp_status = 0;
                            }
                            //+ZIPCLOSE: 0
                            if (memcmp(_response_buf, "+ZIPCLOSE:", 10) == 0) {
                                char *p = &_response_buf[11];
                                int tid = atoi(p);
                                if (tid >= 0 && tid < 5) {
                                    gsm_tcp_close(tid);
                                }
                            }
                            
                            /*if (memcmp(_response_buf, "+CPIN: READY", 12)==0) {
                            gsm_info.gsm_status = 2;
                            gsm_info.simcard_status = 1;
                        }*/    
                            _response_offset = 0;
                        }
                    }
                }
        }
        
        /**/
        ch = mg2639_get_ch();
    }
}
//////////////////////////////////////////////////////
struct _at_t{
    struct pt _pt;
    char status;
    struct process *cp;
}_at;
process_event_t event_at_finish;
/*********************************************************************************************
* 名称：void _rs_at_test(char *rsp)
* 功能：测试
* 参数：
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void _rs_at_test(char *rsp)
{
    if (rsp == NULL) {
        printf("timeout..\r\n");
        
        _at.status = 0;
    } else {
        
        _at.status = 0;
    }
    //process_post(_at.cp, PROCESS_EVENT_NONE, (void *)1);
}
/*********************************************************************************************
* 名称：PT_THREAD(_at_thread(process_event_t ev, void *data))
* 功能：PT进程线
* 参数：
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
PT_THREAD(_at_thread(process_event_t ev, void *data))
{
    
    PT_BEGIN(&_at._pt);
    
    //_at.cp = PROCESS_CURRENT();
    _at.status = 1;
    printf("at start...\r\n");
    _request_at_3("at\r\n", 3000, _rs_at_test);
    PT_WAIT_UNTIL(&_at._pt, _at.status == 0);
    printf("at finish....\r\n");
    
    PT_END(&_at._pt);
}
/*********************************************************************************************
* 名称：voidat_test(void)
* 功能：测试
* 参数：
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
void at_test(void)
{
    PT_INIT(&_at._pt);
    _at_thread(PROCESS_EVENT_NONE,NULL);
}


#include "shell.h"
PROCESS(shell_gsm_process, "gsm shell process");
SHELL_COMMAND(gsm_command, "gsm", "gsm: gsm operator",
              &shell_gsm_process);
/*********************************************************************************************
* 名称：PROCESS_THREAD(shell_gsm_process, ev, data)
* 功能：进程线
* 参数：
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
PROCESS_THREAD(shell_gsm_process, ev, data)
{
    
    PROCESS_BEGIN();
    
    //printf("gsm data %s\r\n", data);
    char buf[64];
    if (strlen(data) > 0) {
        sprintf(buf, "%u\r\n", data);
        _request_at_3(buf, 1000, NULL);
    } else 
        gsm_send_message("13135308483", "abc");
    PROCESS_EXIT();
    
    PROCESS_END();
}
/*********************************************************************************************
* 名称：PROCESS_THREAD(gsm, ev, data)
* 功能：GSM进程线
* 参数：
* 返回：无
* 修改：
* 注释：
*********************************************************************************************/
PROCESS_THREAD(gsm, ev, data)
{
    static struct etimer et;
    PROCESS_BEGIN();                                              //开始进程
    if(connect_mode == 1)
    {
        mg2639_init();                                                //gsm模块初始化
        event_at_finish = process_alloc_event();
        
        etimer_set(&et, CLOCK_SECOND/10); // 100 ms
        PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);           //等待事件触发
        
        //mytimer_set(&gsm_timer, _gsm_timer_call, NULL);
        //mytimer_add(&gsm_timer, 1000);
        ctimer_set(&gsm_timer, 1000, _gsm_timer_call, NULL);          //设置定时器
        shell_register_command(&gsm_command);                         //登录指令
        while (1) 
        {
            _poll_request();                                            //轮询请求 
            _poll_response();                                           //轮询响应
            etimer_set(&et, CLOCK_SECOND/100);                          //定时器设置
            PROCESS_YIELD();                                            //切换进程
        }
    }
    PROCESS_END();                                                //结束进程
}