#include "zxbee.h"
#include "sample.h"
#include "rtc.h"
#include "config.h"
#include "alarm.h"


/*–¥BUF*/
#define BUFSIZE 128
static char wbuf[BUFSIZE];


void zxbeeBegin(void)
{
    wbuf[0] = '{';
    wbuf[1] = '\0';
}


int zxbeeAdd(char* tag, char* val)
{
    int offset = strlen(wbuf);
    sprintf(&wbuf[offset],"%s=%s,", tag, val);
    return 0;
}


char* zxbeeEnd(void)
{
    int offset = strlen(wbuf);
    wbuf[offset-1] = '}';
    wbuf[offset] = '\0';
    if (offset > 2) return wbuf;
    return NULL;
}



int z_process_command_call(char *tag, char *val)
{
    /*¥¶¿Ì√¸¡Ó*/
    sample_z_process_command_call(tag, val);
    
    return 0;
}



void zxbee_onrecv_fun(char *pkg, int len)
{
    char *p;
    char *ptag = NULL;
    char *pval = NULL;
    
    if (pkg[0] != '{' || pkg[len-1] != '}') return;
    pkg[len-1] = 0;
    p = pkg+1;
    
    do 
    {
        ptag = p;
        p = strchr(p, '=');
        if (p != NULL) 
        {
            *p++ = 0;
            pval = p;
            p = strchr(p, ',');
            if (p != NULL) 
                *p++ = 0;
            z_process_command_call(ptag, pval);
        }
    } 
    while (p != NULL);
    
}

