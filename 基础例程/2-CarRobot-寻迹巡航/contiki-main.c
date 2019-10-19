#include <stdio.h>
#include <contiki.h>
#include <contiki-net.h>
#include <dev/leds.h>
#include "usart.h"
#include "adc_key.h"

unsigned int idle_count = 0;
void debug_init(void (*f)(char));

void main(void)
{
    clock_init();
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    uart1_init(115200);
    debug_init(uart1_putc);
    
    printf("\r\nStarting ");
    printf(CONTIKI_VERSION_STRING);
    printf(" on STM32F40x\r\n");
    
    process_start(&etimer_process, NULL);
    ctimer_init();
    
    
#if AUTOSTART_ENABLE
    autostart_start(autostart_processes);
#endif
    
    while(1) 
    {
        do 
        {} 
        while(process_run() > 0);
        idle_count++;
        /* Idle! */
        /* Stop processor clock */
        /* asm("wfi"::); */
    }
}