/*
 *  SRAM Driver for IS25LP080D
 *  Allen Jiang
 *  319H: Intro to Embedded Systems
 *  February 2020
 */

#include <stdint.h>
#include <stdio.h>
#include "metadata.h"

#ifdef __cplusplus
extern "C" {
#endif

struct AudioSendable {
    uint16_t soundIndex;
    uint32_t startIndex;
    int32_t endIndex;   // if endIndex == -1, play entire song
    uint32_t frames;
    FILE* file;
};

//  DAC pins, smaller index == smaller resistor value
const uint32_t dac_pins[8][2] = {
                           {GPIO_PORTA_BASE, GPIO_PIN_4},
                           {GPIO_PORTA_BASE, GPIO_PIN_2},
                           {GPIO_PORTD_BASE, GPIO_PIN_6},
                           {GPIO_PORTC_BASE, GPIO_PIN_7},
                           {GPIO_PORTC_BASE, GPIO_PIN_6},
                           {GPIO_PORTB_BASE, GPIO_PIN_3},
                           {GPIO_PORTE_BASE, GPIO_PIN_3},
                           {GPIO_PORTE_BASE, GPIO_PIN_2},
};

extern void audioISR(UArg arg);
extern void ReadSDFIFO();
extern void Audio_init();
extern void Audio_initSD();
extern void Audio_closeSD();
extern int8_t Audio_playSendable(struct AudioSendable sendable);
extern void Audio_destroySendable(int8_t slotID);
extern void Audio_DAC_write(uint16_t mapping);

#ifdef __cplusplus
}
#endif
