#include <contiki.h>

PROCESS_NAME(led);
PROCESS_NAME(lcd);
PROCESS_NAME(htu21d_update);
PROCESS_NAME(user);

struct process * const autostart_processes[] = {
    &led,
    &lcd,
    &htu21d_update,
    &user,            //用户进程
    NULL
};
