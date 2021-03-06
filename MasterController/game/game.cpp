
#include <xdc/runtime/System.h>
#include "Utils.h"
#include "UART.h"
#include "metadata.h"
#include "entities.h"
#include "stage.h"
#include "Controller.h"
#include "game.h"
#include "Audio.h"
#include "v_tm4c123gh6pm.h"
#include "charactermenu.h"
#include "stagemenu.h"
#include "WinScreen.h"
#include "Title.h"

using namespace std;

Title titleScreen;
StageMenu stageMenu;
CharacterMenu characterMenu;
WinScreen winScreen;

Player* p1;
Player* p2;
Stage stage;
HitboxManager hitboxManager;

Valvano valvano1;
Valvano valvano2;

Kirby kirby1;
Kirby kirby2;

GameandWatch gameandwatch1;
GameandWatch gameandwatch2;

int8_t winner, winningCharacter;
int8_t p1char, p2char;
bool english = true;
bool inTitleScreen = true, inCharMenu = false, inStageSelect = true, inWinScreen = false;
bool countdown, gameOver;
uint8_t frameIndex, frameLength;
long long loopsCompleted;

int8_t backgroundAudioHandle = -1, countdownAudioHandle = -1, gameEndAudioHandle = -1;

float x = 0;
float y = 0;

const bool PLAYER2 = true;
const double UPDATERATE = 20;   // 20

uint8_t stageToPlay;

void resetPlayers() {
    Audio_destroy(&gameEndAudioHandle);
    Audio_clearBuffer();

    p1->setPlayer(1);
    p1->setX(stage.getStartX(1));
    p1->setY(stage.getStartY(1));
    p1->setMirrored(false);
    p1->setStocks(3);
    p1->reset();

    p2->setPlayer(2);
    p2->setX(stage.getStartX(2));
    p2->setY(stage.getStartY(2));
    p2->setMirrored(true);
    p2->setStocks(3);
    p2->reset();

    countdown = true;
    loopsCompleted = 0;
    gameOver = false;

    frameIndex = 0;
    frameLength = 0;

    p1->controlLoop(0,0,0,0,0, &stage, &hitboxManager);
    if(PLAYER2) p2->controlLoop(0,0,0,0,0, &stage, &hitboxManager);
    UART_commandUpdate();
}

//  runs once at beginning
void game_startup() {
    if(PLAYER2) hitboxManager.initialize(p1, p2);
    else hitboxManager.initialize(p1);
    stage.initialize(stageToPlay, &hitboxManager);
    resetPlayers();

    UART_setBackgroundColors(stageToPlay);

    UART_readPersistentSprite(stageToPlay, 0, 0);


    if(p1char == 0 || p2char == 0) UART_readCharacterSDCard(0);
    if(p1char == 1 || p2char == 1) UART_readCharacterSDCard(1);
    if(p1char == 2 || p2char == 2) UART_readCharacterSDCard(2);
    UART_readCharacterSDCard(3);

    sleep(1000);

    //  stop menu music, play stage background music
    if(stageToPlay == STAGE_FINALDESTINATION)
        Audio_play(STAGE_SOUND_FINALDESTINATION, 0.4, &backgroundAudioHandle, 0, -1, true);
    else if(stageToPlay == STAGE_TOWER)
        Audio_play(STAGE_SOUND_UTTOWER, 0.4, &backgroundAudioHandle, 0, -1, true);
    else if(stageToPlay == STAGE_BATTLEFIELD)
        Audio_play(STAGE_SOUND_BATTLEFIELD, 0.4, &backgroundAudioHandle, 0, -1, true);
    else if(stageToPlay == STAGE_SMASHVILLE)
        Audio_play(STAGE_SOUND_SMASHVILLE, 0.4, &backgroundAudioHandle, 0, -1, true);
    else if(stageToPlay == STAGE_EER)
        Audio_play(STAGE_SOUND_EER, 0.4, &backgroundAudioHandle, 0, -1, true);
    else if(stageToPlay == STAGE_GREGORYGYM)
        Audio_play(STAGE_SOUND_GREGORYGYM, 0.4, &backgroundAudioHandle, 0, -1, true);
}

//  continually loops
uint32_t  lastUpdate = 0;
void game_loop() {
    SpriteSendable s;
    lastUpdate = millis();

    if(countdown) {
        if(frameLength++ == 1) {
            frameIndex++;
            frameLength = 0;
        }
        if(frameIndex == 0) {
            Audio_play(1, 1.0, &countdownAudioHandle);     // play countdown
        }
        if(frameIndex == 36) {
            countdown = false;
            Audio_destroy(&countdownAudioHandle);
        }
        else {
            s.x = 100;
            s.y = 100;
            s.charIndex = 3;
            s.framePeriod = 1;
            s.animationIndex = 9;
            s.frame = frameIndex;
            s.persistent = false;
            s.continuous = false;
            s.layer = LAYER_OVERLAY;
            s.mirrored = false;
            UART_sendAnimation(s);
        }
    }

    stage.update();

    if(gameOver) {
        if(p2->dead) p1->controlLoop(0,0,0,0,0, &stage, &hitboxManager);
        else if(PLAYER2 && p1->dead) p2->controlLoop(0,0,0,0,0, &stage, &hitboxManager);
    }
    else if(countdown || gameOver) {
        //  freeze players
        p1->controlLoop(0,0,0,0,0, &stage, &hitboxManager);
        if(PLAYER2) p2->controlLoop(0,0,0,0,0, &stage, &hitboxManager);
    } else {

        p1->controlLoop(
                getJoystick_h(1), getJoystick_v(1),
                getBtn_a(1), getBtn_b(1),
                getBtn_l(1) || getBtn_r(1), &stage,
                &hitboxManager
        );

        if (PLAYER2) {
            p2->controlLoop(
                    getJoystick_h(2), getJoystick_v(2),
                    getBtn_a(2), getBtn_b(2),
                    getBtn_l(2) || getBtn_r(2), &stage,
                    &hitboxManager
            );
        }
    }

    bool updateScore = false;
    if(!p1->dead && !gameOver && (p1->x < -40 || p1->x > 360 || p1->y < -40 || p1->y > 280)) {
        p1->dead = true;
        if(p1->stocksRemaining > 0) {
            p1->stocksRemaining--;
            updateScore = true;
        }
        else {
            gameOver = true;
            frameIndex = 0;
            frameLength = 0;
            Audio_destroyAudio(&backgroundAudioHandle, true);
            Audio_play(4, 0.9, &gameEndAudioHandle);
        }
    }
    if(!p2->dead && !gameOver && (p2->x < -40 || p2->x > 360 || p2->y < -40 || p2->y > 280)) {
        p2->dead = true;
        if(p2->stocksRemaining > 0) {
            p2->stocksRemaining--;
            updateScore = true;
        }
        else {
            gameOver = true;
            frameIndex = 0;
            frameLength = 0;
            Audio_destroyAudio(&backgroundAudioHandle, true);
            Audio_play(4, 0.9, &gameEndAudioHandle);
        }
    }

    if(gameOver) {
        if(frameLength++ == 1) {
            frameIndex++;
            frameLength = 0;
        }
        if(frameIndex == 25) {
            gameOver = false;
            Audio_destroyAllAudio();
            if(p1->dead) switchGameToWin(2, p2char);
            else switchGameToWin(1, p1char);
        }
        else {
            s.x = 80;
            s.y = 120;
            s.charIndex = 3;
            s.framePeriod = 1;
            s.animationIndex = 10;
            s.frame = frameIndex;
            s.persistent = false;
            s.continuous = false;
            s.layer = LAYER_OVERLAY;
            s.mirrored = false;
            UART_sendAnimation(s);
        }
    }
    else if(updateScore) {
        s.charIndex = 3;
        s.framePeriod = 20;
        s.frame = 0;
        s.persistent = false;
        s.continuous = false;
        s.layer = LAYER_PERCENTAGE;
        s.mirrored = false;

        s.x = 90;
        s.y = 100;
        if(p1->stocksRemaining == 3) s.animationIndex = 3;
        else if(p1->stocksRemaining == 2) s.animationIndex = 2;
        else if(p1->stocksRemaining == 1) s.animationIndex = 1;
        else s.animationIndex = 0;
        UART_sendAnimation(s);

        s.x = 170;
        s.y = 100;
        if(p2->stocksRemaining == 3) s.animationIndex = 3;
        else if(p2->stocksRemaining == 2) s.animationIndex = 2;
        else if(p2->stocksRemaining == 1) s.animationIndex = 1;
        else s.animationIndex = 0;
        UART_sendAnimation(s);

        s.x = 140;
        s.y = 115;
        s.animationIndex = 4;
        UART_sendAnimation(s);
    }

    UART_commandUpdate();
    hitboxManager.checkCollisions();

    loopsCompleted++;
}

void startup() {
    UART_commandReset();

    if(inTitleScreen) {
        Audio_destroyAudio(&backgroundAudioHandle, true);
        Audio_play(MENU_SOUND_WORLDOFLIGHT, 0.7, &backgroundAudioHandle);
        titleScreen.start();
    }
    else if(inStageSelect) {
        //  start looping menu music
        Audio_destroyAudio(&backgroundAudioHandle, true);

        stageMenu.start(english, &backgroundAudioHandle);
    }
    else if(inCharMenu) {
        characterMenu.start();
    }
    else if(inWinScreen) {
        winScreen.start(winner, winningCharacter, english);
    }
    else {
        Audio_destroyAudio(&backgroundAudioHandle, true);
        game_startup();
    }
}

void switchTitleToStageMenu() {
    Audio_destroyAudio(&backgroundAudioHandle, true);
    Audio_play(MENU_SOUND_CONFIRM, 0.7);
    inTitleScreen = false;
    inStageSelect = true;
    startup();
}

void switchStageMenuToCharMenu(int8_t stageSelect, bool isEnglish) {
    inStageSelect = false;
    inCharMenu = true;
    english = isEnglish;
    stageToPlay = stageSelect;
    startup();
}

void switchCharMenuToGame(int8_t char1, int8_t char2) {
    if(char1 == CHARACTER_KIRBY) p1 = &kirby1;
    else if(char1 == CHARACTER_VALVANO) p1 = &valvano1;
    else p1 = &gameandwatch1;

    if(char2 == CHARACTER_KIRBY) p2 = &kirby2;
    else if(char2 == CHARACTER_VALVANO) p2 = &valvano2;
    else p2 = &gameandwatch2;

    p1char = char1;
    p2char = char2;

    inCharMenu = false;
    startup();
}

void switchGameToWin(int8_t survivor, int8_t character) {
    winner = survivor;
    winningCharacter = character;

    inWinScreen = true;
    startup();
}

void switchWinToStageMenu() {
    inWinScreen = false;
    inStageSelect = true;

    startup();
}


void loop() {
    if(millis() - lastUpdate >= 1./UPDATERATE*1000) {
        lastUpdate = millis();

        if(inTitleScreen) {
                    titleScreen.loop(getBtn_a(1) || getBtn_a(2) || getBtn_b(1) || getBtn_b(2)
                    || getBtn_start(1) || getBtn_start(2), &switchTitleToStageMenu);
                }
        else if(inStageSelect) {
            stageMenu.loop(getJoystick_h(1), getJoystick_v(1),
                           getJoystick_h(2), getJoystick_v(2),
                           getBtn_a(1) || getBtn_a(2),
                           &switchStageMenuToCharMenu);
        }
        else if(inCharMenu) {
            characterMenu.loop(getJoystick_h(1), getJoystick_v(1),
                               getJoystick_h(2), getJoystick_v(2),
                               getBtn_a(1), getBtn_a(2),
                               getBtn_b(1), getBtn_b(2),
                               getBtn_start(1) || getBtn_start(2),
                               &switchCharMenuToGame);
        }
        else if(inWinScreen) {
            winScreen.loop(&switchWinToStageMenu);
        }
        else {
            game_loop();
        }
    }
}
