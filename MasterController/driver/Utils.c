
/* XDCtools Header files */
#include <driver/Board.h>
#include <driver/Utils.h>
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

bool seedSet = false;

void sleep(uint32_t ms) {
    Task_sleep(ms);
}

uint32_t millis() {
    return Clock_getTicks();
}

double absVal(double d) {
    if(d < 0) return -d;
    else return d;
}

// range of at most 100 because this rng sucks
int random(int low, int high) {
    if(!seedSet) srand(millis());
    int random = rand();

    int toReturn = (random % 100) * (high - low + 1) / 99 + low;

    //  jank code to be safe and because i'm bad
    if(toReturn < low) toReturn = low;
    else if(toReturn > high) toReturn = high;

    return toReturn;
}
