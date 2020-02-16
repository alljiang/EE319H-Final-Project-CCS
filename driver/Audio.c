/*
 *  Audio Driver
 *  Allen Jiang
 *  319H: Intro to Embedded Systems
 *  February 2020
 */

/* XDC module Headers */
#include <driver/Board.h>
#include <xdc/std.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <xdc/runtime/Diags.h>
#include <xdc/runtime/System.h>

/* TI-RTOS Header files */
#include <ti/drivers/GPIO.h>

#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

#include "Audio.h"


void Audio_init() {
    uint8_t i;
    for(i = 0; i < 8; i++) {
        GPIOPinTypeGPIOOutput(dac_pins[i][0], dac_pins[i][1]);
    }
}

//  8 bit, MSB == highest resistance
void Audio_write(uint16_t mapping) {
    uint8_t i;
    for(i = 0; i < 8; i++) {
        uint8_t output = (mapping >> i) & 1;
        if(output) output = dac_pins[i][1];
        GPIOPinWrite(dac_pins[i][0], dac_pins[i][1], output);
    }
}
