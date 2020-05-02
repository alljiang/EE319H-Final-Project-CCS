//
// Created by Allen on 2/29/2020.
//

#include "stage.h"
#include "metadata.h"
#include "entities.h"
#include "UART.h"

//  if damage == 0, corner is left. else, it's right corner
const Hurtbox FD_leftCorner = Hurtbox(true, 38, 103,
                                      SHAPE_CIRCLE, 8,
                                      1, 0,0, 0, 0);
const Hurtbox FD_rightCorner = Hurtbox(
        true, 275, 103, SHAPE_CIRCLE,8,
        1, 1,1, 0, 0);

const Hurtbox BF_leftCorner = Hurtbox(true, 52, 85,
                                      SHAPE_CIRCLE, 8,
                                      1, 0,0, 0, 0);
const Hurtbox BF_rightCorner = Hurtbox(
        true, 268, 85, SHAPE_CIRCLE,8,
        1, 1,1, 0, 0);

const Hurtbox SV_leftCorner = Hurtbox(true, 10, 63,
                                      SHAPE_CIRCLE, 11,
                                      1, 0,0, 0, 0);
const Hurtbox SV_rightCorner = Hurtbox(
        true, 287, 61, SHAPE_CIRCLE,8,
        1, 1,1, 0, 0);

double Stage::ceil(double x, double y) {

    switch(stageIndex) {

        case STAGE_FINALDESTINATION:
            return CEIL_MAX;
//            if(x <= 38 || x >= 288 || y >= 104) return CEIL_MAX;
//            else if(x <= 159) return -0.6198 * x + 93.55;
//            else return .6198 * x -103.55;

        case STAGE_TOWER:
            return CEIL_MAX;

        case STAGE_BATTLEFIELD:
            return CEIL_MAX;
//            if(x <= 52 || x >= 268 || y >= 86) return CEIL_MAX;
//            else if(x <= 161) return -.5505* x + 83.624;
//            else return .5607 * x -95.28;

        case STAGE_SMASHVILLE:
            if(x <= 232 && x >= 100 && y <= 50) return 5;
            else return CEIL_MAX;

        case STAGE_EER:
            return CEIL_MAX;

        case STAGE_GREGORYGYM:
            return CEIL_MAX;
    }
    return CEIL_MAX;
}

double Stage::floor(double x, double y) {

    switch(stageIndex) {

        case STAGE_FINALDESTINATION:
            if(x >= 38 && x <= 288 && y >= 104) return 104;
            else return FLOOR_MAX;

        case STAGE_TOWER:
            return 1;

        case STAGE_BATTLEFIELD:
            if(y >= 151 && x >= 135 && x <= 185) return 151;
            if(y >= 119 && x >= 76 && x <= 128) return 119;
            if(y >= 119 && x >= 193 && x <= 244) return 119;
            if(x >= 52 && x <= 268 && y >= 86) return 86;
            else return FLOOR_MAX;

        case STAGE_SMASHVILLE:
            if(x >= sv_platform_x && x <= sv_platform_x + SMASHVILLE_PLATFORM_LENGTH
                && y >= sv_platform_y + SMASHVILLE_PLATFORM_HEIGHT) return sv_platform_y + SMASHVILLE_PLATFORM_HEIGHT;
            if(x >= 35 && x <= 295 && y >= 65) return 65;
            else return FLOOR_MAX;

        case STAGE_EER:
            if(y >= 167 && x >= 94 && x <= 241) return 167;
            if(y >= 139 && x >= 93 && x <= 242) return 139;
            if(y >= 108 && x >= 92 && x <= 243) return 108;
            if(y >= 77 && x >= 91 && x <= 244) return 77;
            if(y >= 43 && x >= 88 && x <= 247) return 43;
            return FLOOR_MAX;

        case STAGE_GREGORYGYM:
            if(y >= 164 && x >= 138 && x <= 190) return 164;
            if(y >= 53 && x >= 78 && x <= 248) return 53;
            else return 1;
    }
    return FLOOR_MAX;
}

double Stage::rightBound(double x, double y) {
    switch(stageIndex) {

        case STAGE_FINALDESTINATION:
            if(x < 38 && y < 104 && y > 70) return 38;
            if(y < 70 && x <= 159) return (y - 93.55) / -0.6198;
            return RIGHT_MAX;

        case STAGE_TOWER:
            return RIGHT_MAX;

        case STAGE_BATTLEFIELD:
            if(x < 52 && y < 86 && y > 55) return 52;
            if(y <= 55 && x <= 161) return (y - 83.624) / -0.5505;
            return RIGHT_MAX;

        case STAGE_SMASHVILLE:
            if(y < 65 && y >= 20 && x <= 80 && x >= 13) return (y - 75.38) / -.6923;
            return RIGHT_MAX;

        case STAGE_EER:
            return RIGHT_MAX;

        case STAGE_GREGORYGYM:
            return RIGHT_MAX;
    }
    return RIGHT_MAX;
}

double Stage::leftBound(double x, double y) {
    switch(stageIndex) {

        case STAGE_FINALDESTINATION:
            if(x > 280 && y < 104 && y > 70) return 280;
            if(y <= 70 && x >= 159) return (y + 103.55) / 0.6198;
            return LEFT_MAX;

        case STAGE_TOWER:
            return LEFT_MAX;

        case STAGE_BATTLEFIELD:
            if(x > 268 && y < 86 && y > 55) return 268;
            if(y <= 55 && x >= 161) return (y + 95.28) / 0.5607;
            return LEFT_MAX;

        case STAGE_SMASHVILLE:
            if(y < 65 && y >= 5 && x >= 232) return (y+215.95)/0.9524;
            return LEFT_MAX;

        case STAGE_EER:
            return LEFT_MAX;

        case STAGE_GREGORYGYM:
            return LEFT_MAX;
    }
    return LEFT_MAX;
}

double Stage::getStartX(uint8_t player) {
    switch(stageIndex) {
        case STAGE_FINALDESTINATION:
            if(player == 1) return 60;
            else return 232;
        case STAGE_TOWER:
            if(player == 1) return 60;
            else return 232;

        case STAGE_BATTLEFIELD:
            if(player == 1) return 60;
            else return 232;

        case STAGE_SMASHVILLE:
            if(player == 1) return 60;
            else return 232;

        case STAGE_EER:
            if(player == 1) return 100;
            else return 192;

        case STAGE_GREGORYGYM:
            if(player == 1) return 60;
            else return 200;
    }
    return 0;
}

double Stage::getStartY(uint8_t player) {
    switch(stageIndex) {
        case STAGE_FINALDESTINATION:
            return 104;

        case STAGE_TOWER:
            return 1;

        case STAGE_BATTLEFIELD:
            return 86;

        case STAGE_SMASHVILLE:
            return 65;

        case STAGE_EER:
            if(player == 1) return 139;
            else return 77;

        case STAGE_GREGORYGYM:
            if(player == 1) return 1;
            else return 53;
    }
    return 0;
}

bool Stage::onPlatform(double x, double y) {
    switch(stageIndex) {
        case STAGE_FINALDESTINATION:
            return false;

        case STAGE_TOWER:
            return false;

        case STAGE_BATTLEFIELD:
            if(y == 119 && x >= 76 && x <= 128) return true;
            if(y == 151 && x >= 135 && x <= 185) return true;
            if(y == 119 && x >= 193 && x <= 244) return true;

        case STAGE_SMASHVILLE:
            if(x >= sv_platform_x && x <= sv_platform_x + SMASHVILLE_PLATFORM_LENGTH
               && y == sv_platform_y + SMASHVILLE_PLATFORM_HEIGHT) return true;
            return false;

        case STAGE_EER:
            if(y == 167 && x >= 94 && x <= 241) return true;
            if(y == 139 && x >= 93 && x <= 242) return true;
            if(y == 108 && x >= 92 && x <= 243) return true;
            if(y == 77 && x >= 91 && x <= 244) return true;
            if(y == 43 && x >= 88 && x <= 247) return true;

        case STAGE_GREGORYGYM:
            if(y == 164 && x >= 138 && x <= 190) return true;
            if(y == 53 && x >= 78 && x <= 248) return true;
            else return false;
    }
    return false;
}

double Stage::xVelocity(double x, double y) {
    switch(stageIndex) {
        case STAGE_FINALDESTINATION:
            return 0;

        case STAGE_TOWER:
            return 0;

        case STAGE_BATTLEFIELD:
            return 0;

        case STAGE_SMASHVILLE:
            if(onPlatform(x, y)) return sv_platform_xvel;
            return 0;

        case STAGE_EER:
            return 0;

        case STAGE_GREGORYGYM:
            return 0;
    }
    return 0;
}

void Stage::update() {
    switch(stageIndex) {

        case STAGE_FINALDESTINATION:
            break;

        case STAGE_TOWER:
            break;

        case STAGE_BATTLEFIELD:
            break;

        case STAGE_SMASHVILLE:
            SpriteSendable s;
            s.charIndex = 3;
            s.animationIndex = 20;
            s.framePeriod = 1;
            s.frame = 0;
            s.persistent = false;
            s.continuous = false;
            s.x = (int16_t) sv_platform_x;
            s.y = (int16_t) sv_platform_y;
            s.layer = LAYER_NAMETAG;
            s.mirrored = false;
            UART_sendAnimation(s);

            if(sv_platform_goingRight) {
                if(sv_platform_x > SMASHVILLE_PLATFORM_DECELPOINTRIGHT) sv_platform_xvel -= SMASHVILLE_PLATFORM_ACCELERATON;
                else sv_platform_xvel += SMASHVILLE_PLATFORM_ACCELERATON;
                if(sv_platform_xvel > SMASHVILLE_PLATFORM_MAXSPEED) sv_platform_xvel = SMASHVILLE_PLATFORM_MAXSPEED;
                else if(sv_platform_xvel < 0) {
                    sv_platform_goingRight = false;
                    sv_platform_xvel = 0;
                }
                sv_platform_x += sv_platform_xvel;

            } else {
                if(sv_platform_x < SMASHVILLE_PLATFORM_DECELPOINTLEFT) sv_platform_xvel += SMASHVILLE_PLATFORM_ACCELERATON;
                else sv_platform_xvel -= SMASHVILLE_PLATFORM_ACCELERATON;
                if(sv_platform_xvel < -SMASHVILLE_PLATFORM_MAXSPEED) sv_platform_xvel = -SMASHVILLE_PLATFORM_MAXSPEED;
                else if(sv_platform_xvel > 0) {
                    sv_platform_goingRight = true;
                    sv_platform_xvel = 0;
                }
                sv_platform_x += sv_platform_xvel;
            }
            break;

        case STAGE_EER:
            break;

        case STAGE_GREGORYGYM:
            break;

    }
}

void Stage::initialize(uint8_t index, HitboxManager *hitboxManager) {
    stageIndex = index;

    if(stageIndex == STAGE_FINALDESTINATION) {
        hitboxManager->addHurtboxFullConfig(0, 0, false,
                                            FD_leftCorner, 0, true);
        hitboxManager->addHurtboxFullConfig(0, 0, false,
                                            FD_rightCorner, 0, true);
    }
    else if(stageIndex == STAGE_TOWER) {}
    else if(stageIndex == STAGE_BATTLEFIELD) {
        hitboxManager->addHurtboxFullConfig(0, 0, false,
                                            BF_leftCorner, 0, true);
        hitboxManager->addHurtboxFullConfig(0, 0, false,
                                            BF_rightCorner, 0, true);
    }
    else if(stageIndex == STAGE_SMASHVILLE) {
        hitboxManager->addHurtboxFullConfig(0, 0, false,
                                            SV_leftCorner, 0, true);
        hitboxManager->addHurtboxFullConfig(0, 0, false,
                                            SV_rightCorner, 0, true);
        sv_platform_goingRight = true;
        sv_platform_x = 20;
        sv_platform_y = 115;
        sv_platform_xvel = 0;
    }
    else if(stageIndex == STAGE_EER) {

    }
    else if(stageIndex == STAGE_GREGORYGYM) {

    }
}
