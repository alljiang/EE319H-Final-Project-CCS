//
// Created by Allen on 5/5/2020.
//

#ifndef EE319K_FINAL_PROJECT_INITIAL_TESTING_TITLE_H
#define EE319K_FINAL_PROJECT_INITIAL_TESTING_TITLE_H

#include "stdlib.h"
#include "stdint.h"
#include "stdio.h"

class Title {

#define TITLE_MESSAGE_X 92
#define TITLE_MESSAGE_Y 44

public:

    void start();
    void loop(bool buttonPressed, void (*transitionCall)(void));
    void reset();

    uint8_t charIndex = 4;
    int8_t frameIndex;

    int8_t frameLengthCounter;
};


#endif //EE319K_FINAL_PROJECT_INITIAL_TESTING_TITLE_H
