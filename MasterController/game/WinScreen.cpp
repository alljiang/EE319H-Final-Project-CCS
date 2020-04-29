//
// Created by Allen on 4/29/2020.
//

#include "WinScreen.h"
#include "metadata.h"
#include "UART.h"

void WinScreen::start(int8_t winner, int8_t character) {
    reset();

    if(winner == 1) {
        if(character == CHARACTER_KIRBY) {
            UART_setBackgroundColors(BACKGROUND_WIN_P1_KIRBY);
            UART_readPersistentSprite(BACKGROUND_WIN_P1_KIRBY, 0, 0);
        }
        else if(character == CHARACTER_GAMEANDWATCH) {
            UART_setBackgroundColors(BACKGROUND_WIN_P1_GAMEANDWATCH);
            UART_readPersistentSprite(BACKGROUND_WIN_P1_GAMEANDWATCH, 0, 0);
        }
        else {
            UART_setBackgroundColors(BACKGROUND_WIN_P1_VALVANO);
            UART_readPersistentSprite(BACKGROUND_WIN_P1_VALVANO, 0, 0);
        }
    }
    else {
        if(character == CHARACTER_KIRBY) {
            UART_setBackgroundColors(BACKGROUND_WIN_P2_KIRBY);
            UART_readPersistentSprite(BACKGROUND_WIN_P2_KIRBY, 0, 0);
        }
        else if(character == CHARACTER_GAMEANDWATCH) {
            UART_setBackgroundColors(BACKGROUND_WIN_P2_GAMEANDWATCH);
            UART_readPersistentSprite(BACKGROUND_WIN_P2_GAMEANDWATCH, 0, 0);
        }
        else {
            UART_setBackgroundColors(BACKGROUND_WIN_P2_VALVANO);
            UART_readPersistentSprite(BACKGROUND_WIN_P2_VALVANO, 0, 0);
        }
    }
}

void WinScreen::loop(void (*transitionCall)(void)) {
    if(++loopCount >= 20 * WINSCREEN_TIME_SECONDS) {
        (*transitionCall)();
    }
}

void WinScreen::reset() {
    loopCount = 0;
}
