/*
 *  SRAM Driver for IS25LP080D
 *  Allen Jiang
 *  319H: Intro to Embedded Systems
 *  February 2020
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

//  DAC pins, smaller index == smaller resistor value
const uint32_t dac_pins[8][2] = {
                           {GPIO_PORTB_BASE, GPIO_PIN_0},
                           {GPIO_PORTB_BASE, GPIO_PIN_1},
                           {GPIO_PORTB_BASE, GPIO_PIN_2},
                           {GPIO_PORTB_BASE, GPIO_PIN_3},
                           {GPIO_PORTB_BASE, GPIO_PIN_4},
                           {GPIO_PORTB_BASE, GPIO_PIN_5},
                           {GPIO_PORTB_BASE, GPIO_PIN_6},
                           {GPIO_PORTB_BASE, GPIO_PIN_7},
};

extern void Audio_init();
extern void Audio_write(uint16_t mapping);

#ifdef __cplusplus
}
#endif
