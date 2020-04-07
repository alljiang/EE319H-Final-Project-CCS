//
// Created by Allen on 2/5/2020.
//

#ifndef EE319K_FINAL_PROJECT_INITIAL_TESTING_ANIMATOR_H
#define EE319K_FINAL_PROJECT_INITIAL_TESTING_ANIMATOR_H

#include <cstdint>

#define ANIMATOR_COLOR_DONOTCHANGE -2
#define ANIMATOR_COLOR_BACKGROUND -1

extern uint32_t readNextNumber(char delimeter, uint8_t* buf);
extern uint16_t readUntil(char delimeter, uint8_t* buf);

extern void animator_update();

extern void animator_animate(uint8_t animation_charIndex, uint8_t animation_animationIndex,
                    int16_t x, int16_t y, uint8_t frame, uint8_t layer, uint8_t persistent,
                    uint8_t continuous, uint8_t framePeriod, bool mirrored);

extern void animator_setBackgroundColors(const uint32_t *backgroundArr);

extern void animator_readPersistentSprite(const char* spriteName, uint16_t x, uint8_t y);

extern void animator_readCharacterSDCard(uint8_t charIndex);

extern void animator_initialize();

#endif //EE319K_FINAL_PROJECT_INITIAL_TESTING_ANIMATOR_H
