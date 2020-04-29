//
// Created by Allen on 4/28/2020.
//

#ifndef EE319K_FINAL_PROJECT_INITIAL_TESTING_STAGEMENU_H
#define EE319K_FINAL_PROJECT_INITIAL_TESTING_STAGEMENU_H

#include "stdlib.h"
#include "stdint.h"
#include "stdio.h"

class StageMenu {

#define STAGEMENU_CURSORSPEED 12

public:

    void start();
    void loop(double joyH1, double joyV1, double joyH2, double joyV2, bool btnSelect, void (*)(int8_t));
    void reset();

    int8_t getStage(double x, double y);

    uint8_t charIndex = 4;
    long long currentTime;

    double cursorX, cursorY;
};


#endif //EE319K_FINAL_PROJECT_INITIAL_TESTING_STAGEMENU_H
