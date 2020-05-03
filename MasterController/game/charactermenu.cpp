//
// Created by Allen on 4/28/2020.
//

#include "charactermenu.h"
#include "UART.h"
#include "metadata.h"

void CharacterMenu::start() {
    reset();
    UART_setBackgroundColors(BACKGROUND_MENU);
    UART_readPersistentSprite(BACKGROUND_MENU, 0, 0);
    UART_readCharacterSDCard(4);
}

void CharacterMenu::loop(double joyH1, double joyV1, double joyH2, double joyV2,
                         bool btnA1, bool btnA2, bool btnB1, bool btnB2, bool btnStart,
                         void (*transitionCall)(int8_t, int8_t)) {
    transitionCall(0,0);
    return;
    double dt = 49;
    currentTime += (uint8_t)dt;

    SpriteSendable s;
    s.charIndex = charIndex;
    s.framePeriod = 1;
    s.persistent = false;
    s.continuous = false;
    s.mirrored = false;

    //  P1 btn A press
    if(!l_btnA1 && btnA1) {
        if(!p1Selected && getCharacter(p1CursorX, p1CursorY) != -1) {
            p1Selected = true;
        }

    }
    //  P1 btn B press
    else if(!l_btnB1 && btnB1) {
        if(p1Selected) p1Selected = false;
    }

    //  P2 btn A press
    if(!l_btnA2 && btnA2) {
        if(!p2Selected && getCharacter(p2CursorX, p2CursorY) != -1) {
            p2Selected = true;
        }
    }
    //  P2 btn B press
    else if(!l_btnB2 && btnB2) {
        if(p2Selected) p2Selected = false;
    }

    //  start button press
    if(btnStart) {
        if(p1Selected && p2Selected) {
            transitionCall(getCharacter(p1CursorX, p1CursorY), getCharacter(p2CursorX, p2CursorY));
            return;
        }
    }

    if(!p1Selected) {
        p1CursorX += CHARMENU_CURSORSPEED * joyH1;
        p1CursorY += CHARMENU_CURSORSPEED * joyV1;

        //  bounds
        if(p1CursorY < 0) p1CursorY = 0;
        if(p1CursorY > 215) p1CursorY = 215;
        if(p1CursorX < 0) p1CursorX = 0;
        if(p1CursorX > 295) p1CursorX = 295;

        int p1CursorSmall_xAnimationOffset = 0;
        int p1CursorSmall_yAnimationOffset = 0;

        s.animationIndex = 0;
        s.frame = 0;
        s.x = (int16_t) p1CursorX + p1CursorSmall_xAnimationOffset;
        s.y = (int16_t) p1CursorY + p1CursorSmall_yAnimationOffset;
        s.layer = LAYER_CHARACTER;

        UART_sendAnimation(s);
    }
    else {
        int p1CursorBig_xAnimationOffset = -3;
        int p1CursorBig_yAnimationOffset = -3;

        s.animationIndex = 1;
        s.frame = 0;
        s.x = (int16_t) p1CursorX + p1CursorBig_xAnimationOffset;
        s.y = (int16_t) p1CursorY + p1CursorBig_yAnimationOffset;
        s.layer = LAYER_CHARACTER_PROJECTILE;

        UART_sendAnimation(s);

        int xCharPreviewOffset, yCharPreviewOffset;
        int8_t selectedCharacter = getCharacter(p1CursorX, p1CursorY);

        //  kirby
        if(selectedCharacter == 0) {
            s.animationIndex = 10;
            xCharPreviewOffset = 5;
            yCharPreviewOffset = 0;
        }
        //  valvano
        else if(selectedCharacter == 2) {
            s.animationIndex = 11;
            xCharPreviewOffset = 0;
            yCharPreviewOffset = -5;
        }
        //  game and watch
        else if(selectedCharacter == 1) {
            s.animationIndex = 12;
            xCharPreviewOffset = -6;
            yCharPreviewOffset = -1;
        }

        s.x = 10 + xCharPreviewOffset;
        s.y = 5 + yCharPreviewOffset;
        s.layer = LAYER_CHARACTER;

        UART_sendAnimation(s);
    }
    if(!p2Selected) {
        p2CursorX += CHARMENU_CURSORSPEED * joyH2;
        p2CursorY += CHARMENU_CURSORSPEED * joyV2;

        if(p2CursorY < 0) p2CursorY = 0;
        if(p2CursorY > 215) p2CursorY = 215;
        if(p2CursorX < 0) p2CursorX = 0;
        if(p2CursorX > 295) p2CursorX = 295;

        int p2CursorSmall_xAnimationOffset = 0;
        int p2CursorSmall_yAnimationOffset = 0;

        s.animationIndex = 2;
        s.frame = 0;
        s.x = (int16_t) p2CursorX + p2CursorSmall_xAnimationOffset;
        s.y = (int16_t) p2CursorY + p2CursorSmall_yAnimationOffset;
        s.layer = LAYER_CHARACTER;

        UART_sendAnimation(s);
    }
    else {
        int p2CursorBig_xAnimationOffset = -3;
        int p2CursorBig_yAnimationOffset = -3;

        s.animationIndex = 3;
        s.frame = 0;
        s.x = (int16_t) p2CursorX + p2CursorBig_xAnimationOffset;
        s.y = (int16_t) p2CursorY + p2CursorBig_yAnimationOffset;
        s.layer = LAYER_CHARACTER_PROJECTILE;

        UART_sendAnimation(s);

        int xCharPreviewOffset, yCharPreviewOffset;
        int8_t selectedCharacter = getCharacter(p2CursorX, p2CursorY);

        //  kirby
        if(selectedCharacter == 0) {
            s.animationIndex = 10;
            xCharPreviewOffset = 5;
            yCharPreviewOffset = 0;
        }
            //  valvano
        else if(selectedCharacter == 2) {
            s.animationIndex = 11;
            xCharPreviewOffset = 0;
            yCharPreviewOffset = -5;
        }
            //  game and watch
        else if(selectedCharacter == 1) {
            s.animationIndex = 12;
            xCharPreviewOffset = -6;
            yCharPreviewOffset = -1;
        }

        s.x = 164 + xCharPreviewOffset;
        s.y = 5 + yCharPreviewOffset;
        s.layer = LAYER_CHARACTER;

        UART_sendAnimation(s);
    }

    UART_commandUpdate();

    updateLastValues(joyH1, joyV1, joyH2, joyV2, btnA1, btnA2, btnB1, btnB2);
}

void CharacterMenu::updateLastValues(double joyH1, double joyV1, double joyH2, double joyV2,
                                     bool btnA1, bool btnA2, bool btnB1, bool btnB2) {
    l_btnA1 = btnA1;
    l_btnB1 = btnB1;
    l_btnA2 = btnA2;
    l_btnB2 = btnB2;
}

void CharacterMenu::reset() {
    p1Selected = p2Selected = false;
    currentTime = 0;
    l_btnA1 = false;
    l_btnB1 = false;
    l_btnA2 = false;
    l_btnB2 = false;
    p1CursorX = 100;
    p2CursorX = 255;
    p1CursorY = p2CursorY = 35;
}

int8_t CharacterMenu::getCharacter(double x, double y) {

    if(y > 197 || y < 80 || x < 25 || x > 270) return -1;

    if(x < 107) return 0;   //  kirby
    else if(x < 194) return 2;  //  valvano
    else return 1;  //  game and watch
}
