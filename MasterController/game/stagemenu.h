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
#define STAGEMENU_ENGLISH_X 83
#define STAGEMENU_ENGLISH_Y 17
#define STAGEMENU_CHINESE_X 166
#define STAGEMENU_CHINESE_Y 17

public:

    void start(bool isEnglish);
    void loop(double joyH1, double joyV1, double joyH2, double joyV2, bool btnSelect, void (*)(int8_t, bool));
    void reset();

    int8_t getStage(double x, double y);
    int8_t getLanguage(double x, double y);

    uint8_t charIndex = 4;
    long long currentTime;

    double cursorX, cursorY;

    bool englishSelected;
};


#endif //EE319K_FINAL_PROJECT_INITIAL_TESTING_STAGEMENU_H
