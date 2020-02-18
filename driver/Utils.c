
/*
 *  ======== uartecho.c ========
 */

/* XDCtools Header files */
#include <driver/Board.h>
#include <driver/Utils.h>
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>

#include <stdint.h>

void sleep(uint32_t ms) {
    ms = msToTicks(ms);    // convert to system cycles
    Task_sleep(ms);
}

void sleepMicros(uint32_t us) {
    us = usToTicks(us);
    Task_sleep(us);
}

uint32_t ticksToMicros(uint32_t ticks) {
    return ticks * 10;
}

uint32_t ticksToMillis(uint32_t ticks) {
    return ticks / 100;
}

uint32_t msToTicks(uint32_t ms) {
    return ms * 100;
}

uint32_t usToTicks(uint32_t us) {
    return us / 10;
}

uint32_t millis() {
    return ticksToMillis(Clock_getTicks());
}

uint32_t micros() {
    return ticksToMicros(Clock_getTicks());
}
