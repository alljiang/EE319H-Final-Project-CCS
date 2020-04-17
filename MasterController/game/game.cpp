
#include "Utils.h"
#include "UART.h"
#include "metadata.h"
#include "entities.h"
#include "stage.h"
#include "Controller.h"
#include "game.h"

using namespace std;

Player* p1;
Player* p2;
Stage stage;
HitboxManager hitboxManager;

Kirby k1;
Kirby k2;

bool quit;

float x = 0;
float y = 0;

const bool PLAYER2 = true;
const double UPDATERATE = 20;   // 20

const uint8_t stageToPlay = STAGE_FINALDESTINATION;
//const uint8_t stageToPlay = STAGE_TOWER;

//  runs once at beginning
void game_startup() {
    p1 = &k1;
    p1->setPlayer(1);
    p1->setX(stage.getStartX(1));
    p1->setY(stage.getStartY(1));
    p1->setStocks(3);

    p2 = &k2;
    k2.setPlayer(2);
    p2->setX(stage.getStartX(2));
    p2->setY(stage.getStartY(2));
    p2->setMirrored(true);
    p2->setStocks(3);

    if(PLAYER2) hitboxManager.initialize(p1, p2);
    else hitboxManager.initialize(p1);

    if(stageToPlay == STAGE_FINALDESTINATION) UART_setBackgroundColors(STAGE_FINALDESTINATION);
    else if(stageToPlay == STAGE_TOWER) UART_setBackgroundColors(STAGE_TOWER);

    stage.initialize(stageToPlay, &hitboxManager);
    UART_readPersistentSprite(stageToPlay, 0, 0);

    UART_readCharacterSDCard(0);
    UART_readCharacterSDCard(3);
}

//  continually loops
uint32_t  lastUpdate = 0;
void game_loop() {
    if(millis() - lastUpdate >= 1./UPDATERATE*1000) {
        lastUpdate = millis();
        stage.update();
        p1->controlLoop(
                getJoystick_h(1), getJoystick_v(1),
                getBtn_a(1), getBtn_b(1),
                getBtn_l(1) || getBtn_r(1), &stage,
                &hitboxManager
                );

        if(PLAYER2) {
            p2->controlLoop(
                    getJoystick_h(2), getJoystick_v(2),
                    getBtn_a(2), getBtn_b(2),
                    getBtn_l(2) || getBtn_r(2), &stage,
                    &hitboxManager
            );
        }

        bool updateScore = false;
        if(!p1->dead && (p1->x < -40 || p1->x > 360 || p1->y < -40 || p1->y > 280)) {
            p1->dead = true;
            updateScore = true;

            if(p1->stocksRemaining > 0) p1->stocksRemaining--;
        }
        if(!p2->dead && (p2->x < -40 || p2->x > 360 || p2->y < -40 || p2->y > 280)) {
            p2->dead = true;
            updateScore = true;

            if(p2->stocksRemaining > 0) p2->stocksRemaining--;
        }

        if(updateScore) {
            SpriteSendable s;
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
    }
}
