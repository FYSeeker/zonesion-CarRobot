#include <contiki.h>

PROCESS_NAME(lcd);
PROCESS_NAME(key);
PROCESS_NAME(led);
PROCESS_NAME(ble_recv);
PROCESS_NAME(alarm);
PROCESS_NAME(qrcode);
PROCESS_NAME(bt_radio);
PROCESS_NAME(gsm);
PROCESS_NAME(utcp);
PROCESS_NAME(config);
PROCESS_NAME(htu21d_update);
PROCESS_NAME(ultrasonic);
PROCESS_NAME(motor);
PROCESS_NAME(robot_arm);
PROCESS_NAME(robotArm_motion);
PROCESS_NAME(user);

struct process * const autostart_processes[] = {
    &config,
    &gsm,
    &utcp,
    &led,
    &key,
    &lcd,
    &ble_recv,
    &qrcode,          //二维码
    &htu21d_update,
    &ultrasonic,
    &motor,
    &robot_arm,
    &robotArm_motion,
    &user,            //用户进程
    NULL
};
