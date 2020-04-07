/*
 *  SRAM Driver for IS25LP080D
 *  Allen Jiang
 *  319H: Intro to Embedded Systems
 *  February 2020
 */

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void Controller_init();
extern void Controller_updateController();
extern double getJoystick_h(uint8_t player);
extern double getJoystick_v(uint8_t player);
extern bool getBtn_a(uint8_t player);
extern bool getBtn_b(uint8_t player);
extern bool getBtn_l(uint8_t player);
extern bool getBtn_r(uint8_t player);
extern bool getBtn_start(uint8_t player);

#ifdef __cplusplus
}
#endif
