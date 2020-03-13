
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
    Task_sleep(ms);
}

uint32_t millis() {
    return Clock_getTicks();
}
