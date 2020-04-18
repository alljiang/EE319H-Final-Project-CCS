/*
 *  Audio Driver
 *  Allen Jiang
 *  319H: Intro to Embedded Systems
 *  February 2020
 */

#include <stdint.h>
#include <stdio.h>
#include <xdc/std.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

const char soundNames[100][10] = {
/*  0   */      {"menu"},
                {"321go"},
                {"smash"},
                {"sine"},
                {"game"},
                {},
                {},
                {},
                {},
                {},
                {},
                {},
                {},
                {},
                {},
                {},
                {},
                {},
                {},
                {},
                {},
                {}
};

struct AudioParams {
    uint16_t soundIndex;
    uint32_t startIndex;
    int32_t endIndex;   // in frames. if endIndex == -1, play entire song
    uint32_t frames;
    int32_t FIFO_size;   // -1 means not set
    float volume;
    bool loop;
    FILE* file;
};

//  DAC pins, smaller index == smaller resistor value
const uint32_t dac_pins[8][2] = {
                           {GPIO_PORTA_BASE, GPIO_PIN_2},
                           {GPIO_PORTE_BASE, GPIO_PIN_3},
                           {GPIO_PORTD_BASE, GPIO_PIN_6},
                           {GPIO_PORTC_BASE, GPIO_PIN_7},
                           {GPIO_PORTC_BASE, GPIO_PIN_6},
                           {GPIO_PORTB_BASE, GPIO_PIN_3},
                           {GPIO_PORTA_BASE, GPIO_PIN_7},
                           {GPIO_PORTF_BASE, GPIO_PIN_4},
};

extern void audioISR(UArg arg);
extern void ReadSDFIFO();
extern void Audio_init();
extern void Audio_initSD();
extern void Audio_closeSD();
extern int8_t Audio_playAudio(struct AudioParams sendable);
extern int8_t Audio_play(uint16_t soundIndex, float volume, uint32_t startIndex=0, int32_t endIndex=-1, bool loop=false);
extern void Audio_destroy(int8_t* slotID);
extern void Audio_destroyAudio(int8_t* slotID, bool overrideLoop);
extern void Audio_destroyAllAudio();
extern void Audio_clearBuffer();
extern void Audio_DAC_write(uint16_t mapping);
extern void Audio_initParams(struct AudioParams* params);

#ifdef __cplusplus
}
#endif
