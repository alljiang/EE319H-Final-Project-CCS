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
#include <ti/sysbios/knl/Clock.h>

#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

#include "Audio.h"

Clock_Struct clk0Struct;

uint32_t smashIndex = 0;

void clk0Fxn(UArg arg0)
{
    if(smashIndex >= 200000) smashIndex = 0;
    uint8_t i;
    for(i = 0; i < 1; i++){}
    Audio_write(smash[smashIndex++]);
}

void Audio_init() {
    //  Set GPIO pins
    uint8_t i;
    for(i = 0; i < 8; i++) {
        GPIOPinTypeGPIOOutput(dac_pins[i][0], dac_pins[i][1]);
    }

    //  Init periodic clock with period = 12us
    Clock_Params clkParams;
    Clock_Params_init(&clkParams);
    clkParams.period = 2;
    clkParams.startFlag = TRUE;

    Clock_construct(&clk0Struct, (Clock_FuncPtr)clk0Fxn, 1, &clkParams);
}

//  8 bit, MSB == highest resistance
void Audio_write(uint16_t mapping) {
    uint8_t i;
    for(i = 0; i < 8; i++) {
        uint8_t output = (mapping >> (7-i)) & 1;
        if(output) output = dac_pins[i][1];
        GPIOPinWrite(dac_pins[i][0], dac_pins[i][1], output);
    }
}
