#include <contiki.h>

PROCESS_NAME(led);
PROCESS_NAME(key);
PROCESS_NAME(lcd);
PROCESS_NAME(config);
PROCESS_NAME(ble_recv);
PROCESS_NAME(qrcode);
PROCESS_NAME(gsm);
PROCESS_NAME(utcp);
PROCESS_NAME(htu21d_update);
PROCESS_NAME(user);

struct process * const autostart_processes[] = {
    &config,
    &led,
    &key,
    &lcd,
    &gsm,
    &utcp,
    &ble_recv,
    &qrcode,          //二维码
    &htu21d_update,
    &user,            //用户进程
    NULL
};
