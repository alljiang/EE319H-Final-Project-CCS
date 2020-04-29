//
// Created by Allen on 4/29/2020.
//

#include "stdlib.h"
#include "stdint.h"
#include "stdio.h"

#ifndef EE319K_FINAL_PROJECT_INITIAL_TESTING_WINSCREEN_H
#define EE319K_FINAL_PROJECT_INITIAL_TESTING_WINSCREEN_H

class WinScreen {

#define WINSCREEN_TIME_SECONDS 5

public:

    int loopCount;

    void start(int8_t winner, int8_t character);
    void loop(void (*)(void));
    void reset();

};


#endif //EE319K_FINAL_PROJECT_INITIAL_TESTING_WINSCREEN_H
