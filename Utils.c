
/*
 *  ======== uartecho.c ========
 */

/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>

#include "Board.h"
#include "Utils.h"

#include <stdint.h>

void delay(uint32_t ms) {
    ms = msToTicks(ms);    // convert to system cycles
    for(; ms > 0; ms--) {}
//    Task_sleep(ms);
}

uint32_t ticksToMicros(uint32_t ticks) {
    return ticks / 6.667;
}

uint32_t ticksToMillis(uint32_t ticks) {
    return ticks / 6667;
}

uint32_t msToTicks(uint32_t ms) {
    return ms * 6667;
}

uint32_t millis() {
    return Clock_getTicks();
}
