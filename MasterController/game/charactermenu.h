//
// Created by Allen on 4/28/2020.
//

#ifndef EE319K_FINAL_PROJECT_INITIAL_TESTING_CHARACTERMENU_H
#define EE319K_FINAL_PROJECT_INITIAL_TESTING_CHARACTERMENU_H

#include "stdlib.h"
#include "stdint.h"
#include "stdio.h"

class CharacterMenu {

#define CHARMENU_CURSORSPEED 9

public:
    bool l_btnA1;                //  P1 last button A value
    bool l_btnB1;                //  P1 last button B value
    bool l_btnA2;                //  P2 last button A value
    bool l_btnB2;                //  P2 last button B value

    void start();
    void loop(double joyH1, double joyV1, double joyH2, double joyV2,
              bool btnA1, bool btnA2, bool btnB1, bool btnB2, bool btnStart,
              void (*)(int8_t, int8_t));
    void reset();
    void updateLastValues(double joyH1, double joyV1, double joyH2, double joyV2,
                          bool btnA1, bool btnA2, bool btnB1, bool btnB2);

    int8_t getCharacter(double x, double y);

    uint8_t charIndex = 4;

    bool p1Selected, p2Selected;
    long long currentTime;

    double p1CursorX;
    double p1CursorY;
    double p2CursorX;
    double p2CursorY;
};


#endif //EE319K_FINAL_PROJECT_INITIAL_TESTING_CHARACTERMENU_H
