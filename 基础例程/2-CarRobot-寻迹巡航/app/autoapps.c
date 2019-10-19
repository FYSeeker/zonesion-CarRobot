#include <contiki.h>

PROCESS_NAME(led);
PROCESS_NAME(key);
PROCESS_NAME(lcd);
PROCESS_NAME(htu21d_update);
PROCESS_NAME(motor);
PROCESS_NAME(user);

struct process * const autostart_processes[] = {
    &led,
    &key,
    &lcd,
    &htu21d_update,
    &motor,
    &user,            //用户进程
    NULL
};
