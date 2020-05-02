//
// Created by Allen on 4/30/2020.
//

#include "entities.h"
#include "utils.h"
#include "metadata.h"
#include "UART.h"
#include "stage.h"

void Valvano::controlLoop(float joyH, float joyV, bool btnA, bool btnB, bool shield,
                        class Stage *stage, class HitboxManager *hitboxManager) {
    //  assume joystick deadzone filtering is already done

    float dt = 49;
    currentTime += (uint8_t)dt;

    SpriteSendable s;

    //  check if dead
    if(dead) {
        if(deathTime == 0) deathTime = currentTime;
        else if(currentTime - deathTime > 1000) {
            //  respawn

            //  reset
            reset();

            y = 240;
            x = stage->getStartX(player);
            invulnerableFrames = 20 * 3;

            dead = false;
            action = VAL_ACTION_FALLING;

            if(player == 2) mirrored = true;
        }
        return;
    }

    if(!l_btnA && btnA) l_btnARise_t = currentTime;
    else if(l_btnA && !btnA) l_btnAFall_t = currentTime;
    if(!l_btnB && btnB) l_btnBRise_t = currentTime;
    else if(l_btnB && !btnB) l_btnBFall_t = currentTime;
    if(!l_shield && shield) l_shieldRise_t = currentTime;
    else if(l_shield && !shield) l_shieldFall_t = currentTime;

    int16_t x_mirroredOffset = 0;

    float yAnimationOffset = 0;
    float xAnimationOffset = 0;

    hitbox.offsetY(0);
    hitbox.offsetX(0);
    hitbox.offsetRadius(0);
    hitbox.offsetWidth(0);
    hitbox.offsetHeight(0);

    float ceiling = stage->ceil(x + VAL_STAGE_OFFSET, y);
    float floor = stage->floor(x + VAL_STAGE_OFFSET, y);
    float leftBound = stage->leftBound(x + VAL_STAGE_OFFSET, y) - VAL_STAGE_OFFSET / 2;
    float rightBound = stage->rightBound(x - VAL_STAGE_OFFSET, y) - VAL_STAGE_OFFSET;
    bool onPlatform = stage->onPlatform(x + VAL_STAGE_OFFSET, y);
    float stageVelocity = stage->xVelocity(x, y);
    float gravityScale = 1;

    //  first, follow up on any currently performing actions

    //  movement
    if(action == VAL_ACTION_JUMPING) {
        hitbox.offsetY(0);
        if(mirrored) hitbox.offsetX(4);
        else hitbox.offsetX(2);
        gravityScale = 1;
        x += airSpeed * joyH;
        if(x > rightBound) x = rightBound;
        else if(x < leftBound) x = leftBound;

        mirrored = l_mirrored;
        animationIndex = 3;
        frameIndex = 0;

        if(yVel <= 0) {
            l_action = VAL_ACTION_JUMPING;
            action = VAL_ACTION_FALLING;
        }
    }
    else if(action == VAL_ACTION_DOUBLEJUMPING) {
        hitbox.offsetY(0);
        if(mirrored) hitbox.offsetX(4);
        else hitbox.offsetX(2);
        gravityScale = 1;
        x += airSpeed * joyH;
        if(x > rightBound) x = rightBound;
        else if(x < leftBound) x = leftBound;

        mirrored = l_mirrored;
        animationIndex = 3;
        frameIndex = 0;

        if(yVel <= 0) {
            l_action = VAL_ACTION_JUMPING;
            action = VAL_ACTION_FALLING;
        }
    }
    else if(action == VAL_ACTION_LEDGEGRAB) {
        animationIndex = 42;
        frameIndex = 0;

        x_mirroredOffset = 0;
        xAnimationOffset = 8;
        yAnimationOffset = -32;

        yVel = 0;

        hitbox.offsetWidth(-11);
        hitbox.offsetHeight(9);
        hitbox.offsetY(-28);
        if(mirrored) hitbox.offsetX(-5);
        else hitbox.offsetX(2);
    }
    else if(action == VAL_ACTION_JAB) {
        animationIndex = 11;
        mirrored = l_mirrored;
        xAnimationOffset = 0;
        x_mirroredOffset = -5;

        disabledFrames = 2;
        frameExtension = 1;

        hitbox.offsetY(0);
        if(mirrored) hitbox.offsetX(0);
        else hitbox.offsetX(0);

        if(frameLengthCounter++ >= frameExtension) {
            frameLengthCounter = 0;
            frameIndex++;
        }
        if(frameIndex >= 8) {
            frameIndex = 0;
            frameLengthCounter = 0;
        }
        if(currentTime-l_btnARise_t > 400) {
            l_action = VAL_ACTION_JAB;
            x_mirroredOffset = 0;
            action = VAL_ACTION_RESTING;

            disabledFrames = 2;
        }

        if(frameIndex == 2 || frameIndex == 5) {
            hitboxManager->addHurtbox(x + 8, y, mirrored,
                                      jab, player);
        }
    }
    else if(action == VAL_ACTION_DASHATTACK) {
        animationIndex = 10;
        mirrored = l_mirrored;
        x_mirroredOffset = -10;
        xAnimationOffset = 0;
        disabledFrames = 2;

        frameExtension = 0;
        if(frameLengthCounter++ >= frameExtension) {
            frameLengthCounter = 0;
            frameIndex++;
        }
        frameIndex %= 5;

        //  I AM SPEED
        double dashAttackSpeed = 4.5;
        if(mirrored) x -= dashAttackSpeed;
        else x += dashAttackSpeed;

        yVel = 0;

        hitbox.offsetWidth(16);
        hitbox.offsetY(0);
        if(mirrored) hitbox.offsetX(-1);
        else hitbox.offsetX(6);

        if(currentTime - dashAttackStartTime > 750) {
            l_action = VAL_ACTION_DASHATTACK;
            if(absVal(joyH) > 0.1) action = VAL_ACTION_RUNNING;
            else action = VAL_ACTION_RESTING;
            disabledFrames = 4;
        }
        else {
            hitboxManager->addHurtbox(x + 10, y, mirrored,
                                      dashAttack, player);
        }
    }
    else if(action == VAL_ACTION_DOWNTILT) {
        animationIndex = 12;
        mirrored = l_mirrored;
        x_mirroredOffset = -20;
        disabledFrames = 2;

        hitbox.offsetY(0);
        hitbox.offsetX(2);
        hitbox.offsetWidth(3);

        frameExtension = 3;
        if(frameLengthCounter++ >= frameExtension) {
            frameLengthCounter = 0;
            frameIndex++;
        }
        if(frameIndex >= 4) {
            l_action = VAL_ACTION_DOWNTILT;
            if(joyV < -0.25) {
                action = VAL_ACTION_CROUCHING;
            }
            else action = VAL_ACTION_RESTING;
            frameIndex = 0;
            disabledFrames = 4;
            x_mirroredOffset = 0;
        }
        else {
            hitboxManager->addHurtbox(x + 18, y, mirrored,
                                      downTilt, player);
        }
    }
    else if(action == VAL_ACTION_FORWARDTILT) {
        animationIndex = 10;
        mirrored = l_mirrored;
        x_mirroredOffset = -20;

        hitbox.offsetY(0);
        hitbox.offsetX(0, mirrored);

        if(frameIndex == 0) frameExtension = 4;
        else if(frameIndex == 1) frameExtension = 7;
        if(frameLengthCounter++ >= frameExtension) {
            frameLengthCounter = 0;
            frameIndex++;
        }
        if(frameIndex >= 2) {
            l_action = VAL_ACTION_FORWARDTILT;
            action = VAL_ACTION_RESTING;
            disabledFrames = 2;
            x_mirroredOffset = 0;
        }
        else {
            if(frameIndex == 1) {
                hitboxManager->addHurtbox(x + 16, y, mirrored,
                                          forwardTilt, player);
            }
        }
    }
    else if(action == VAL_ACTION_UPTILT) {
        animationIndex = 11;
        mirrored = l_mirrored;
        x_mirroredOffset = -12;
        xAnimationOffset = -16;

        frameExtension = 3;
        if(frameLengthCounter++ >= frameExtension) {
            frameLengthCounter = 0;
            frameIndex++;
        }
        if(frameIndex >= 3) {
            l_action = VAL_ACTION_UPTILT;
            action = VAL_ACTION_RESTING;
            disabledFrames = 6;
            x_mirroredOffset = 0;
        }
        else {
            if(frameIndex == 0) {
                hitbox.offsetWidth(6);
                if(mirrored) hitbox.offsetX(0);
                else hitbox.offsetX(6);
            }
            else if(frameIndex == 1) {
                hitbox.offsetWidth(10);
                if(mirrored) hitbox.offsetX(0);
                else hitbox.offsetX(-2);

                hitboxManager->addHurtbox(x + 18, y, mirrored,
                                          upTilt1, player);
            }
            else if(frameIndex == 2) {
                hitbox.offsetWidth(10);
                if(mirrored) hitbox.offsetX(0);
                else hitbox.offsetX(-2);

                hitboxManager->addHurtbox(x + 18, y, mirrored,
                                          upTilt2, player);
            }
        }
    }
    else if(action == VAL_ACTION_FORWARDAIR) {
        if(y == floor) {
            action = VAL_ACTION_RESTING;
            disabledFrames = 4;
        }
        else {
            animationIndex = 21;
            mirrored = l_mirrored;
            gravityScale = 1.0;
            x += airSpeed * joyH * 0.7;

            x_mirroredOffset = -14;
            xAnimationOffset = -11;
            yAnimationOffset = 0;

            hitbox.offsetHeight(-12);
            hitbox.offsetWidth(28);
            hitbox.offsetY(-4);
            if(mirrored) hitbox.offsetX(5);
            else hitbox.offsetX(3);

            disabledFrames = 2;
            if(frameIndex == 3) frameExtension = 3;
            frameExtension = 1;
            if (frameLengthCounter++ >= frameExtension) {
                frameLengthCounter = 0;
                frameIndex++;
            }
            if (frameIndex >= 6) {
                l_action = VAL_ACTION_FORWARDAIR;
                action = VAL_ACTION_FALLING;
                disabledFrames = 4;
            }
            else {
                if(frameIndex == 2 || frameIndex == 3)
                    hitboxManager->addHurtbox(x + 12, y, mirrored,
                                          forwardAir, player);
            }
        }
    }
    else if(action == VAL_ACTION_UPAIR) {
        if(y == floor) {
            action = VAL_ACTION_RESTING;
            disabledFrames = 4;
        }
        else {
            animationIndex = 24;
            mirrored = l_mirrored;
            gravityScale = 1.0;
            x += airSpeed * joyH * 0.4;
            disabledFrames = 2;

            x_mirroredOffset = -4;
            xAnimationOffset = 0;
            yAnimationOffset = 0;

            hitbox.offsetHeight(-2);
            hitbox.offsetY(-1);
            if(mirrored) hitbox.offsetX(-4);
            else hitbox.offsetX(-1);

            frameExtension = 2;
            if (frameLengthCounter++ >= frameExtension) {
                frameLengthCounter = 0;
                frameIndex++;
            }
            if (frameIndex >= 5) {
                l_action = VAL_ACTION_UPAIR;
                action = VAL_ACTION_FALLING;
                disabledFrames = 5;
            }
            else {
                if(frameIndex == 1 || frameIndex == 3) {
                    hitboxManager->addHurtbox(x + 13, y, mirrored,
                                              upAir, player);
                }
            }
        }
    }
    else if(action == VAL_ACTION_DOWNAIR) {
        animationIndex = 22;
        mirrored = l_mirrored;
        x_mirroredOffset = 0;
        xAnimationOffset = 0;
        yAnimationOffset = 0;

        disabledFrames = 2;

        hitbox.offsetX(3);
        hitbox.offsetY(2);
        hitbox.offsetWidth(0);
        hitbox.offsetHeight(4);

        if(frameIndex == 0) frameExtension = 3;
        else if(frameIndex == 1) frameExtension = 4;

        if (frameLengthCounter++ >= frameExtension) {
            frameLengthCounter = 0;
            frameIndex++;
        }
        if(frameIndex >= 2 || y == floor) {
            if(y == floor) action = VAL_ACTION_RESTING;
            else action = VAL_ACTION_FALLING;
        }
        else if(frameIndex == 1) hitboxManager->addHurtbox(x + 11, y, mirrored,
                                      downAir, player);
    }
    else if(action == VAL_ACTION_NEUTRALAIR) {
        if(y <= floor) {
            action = VAL_ACTION_RESTING;
            disabledFrames = 4;
        }
        else {
            animationIndex = 20;
            mirrored = l_mirrored;
            gravityScale = 0.8;
            x += airSpeed * joyH * 0.5;
            x_mirroredOffset = -20;
            xAnimationOffset = -17;
            yAnimationOffset = 0;

            hitbox.offsetHeight(8);
            hitbox.offsetWidth(30);
            hitbox.offsetY(5);
            if(mirrored) hitbox.offsetX(0);
            else hitbox.offsetX(0);

            disabledFrames = 2;
            frameExtension = 1;
            if (frameLengthCounter++ >= frameExtension) {
                frameLengthCounter = 0;
                frameIndex++;
            }
            if (frameIndex >= 4) {
                l_action = VAL_ACTION_NEUTRALAIR;
                action = VAL_ACTION_FALLING;
                disabledFrames = 5;
            }
            else {
                hitboxManager->addHurtbox(x+8, y, mirrored,
                                          neutralAir, player);
            }
        }
    }
    else if(action == VAL_ACTION_UPSPECIAL) {
        if(y <= floor) {
            action = VAL_ACTION_RESTING;
            disabledFrames = 3;
        }
        else {
            animationIndex = 32;
            mirrored = l_mirrored;
            x += airSpeed * joyH * 0.8;
            disabledFrames = 2;

            x_mirroredOffset = -18;
            xAnimationOffset = -18;
            yAnimationOffset = 0;

            hitbox.offsetWidth(4);
            if(mirrored) hitbox.offsetX(1);
            else hitbox.offsetX(3);
            hitbox.offsetY(0);

            frameExtension = 0;
            if(frameLengthCounter++ >= frameExtension) {
                frameIndex++;
                frameLengthCounter = 0;
            }
            frameIndex %= 5;

            if(currentTime - upSpecialStartTime > 2000 || joyV < -0.3) {
                disabledFrames = 5;
                action = VAL_ACTION_FALLING;
                yVel = 0;
                noJumpsDisabled = true;
            }
            else {
                //  go up
                if(yVel > 0) yVel += gravityRising + 0.2;
                else yVel += gravityFalling + 0.4;
                if(yVel > 5) yVel = 5;
            }
        }
    }
    else if(action == VAL_ACTION_SIDESPECIAL) {
        animationIndex = 31;
        mirrored = l_mirrored;
        disabledFrames = 2;

        x_mirroredOffset = -48;
        xAnimationOffset = 0;
        yAnimationOffset = 0;

        if(mirrored) hitbox.offsetX(-2);
        else hitbox.offsetX(3);

        frameExtension = 1;
        if(frameLengthCounter++ >= frameExtension) {
            frameLengthCounter = 0;
            frameIndex++;
        }
        if(frameIndex >= 8) {
            disabledFrames = 1;
            if(y == floor) action = VAL_ACTION_RESTING;
            else action = VAL_ACTION_FALLING;
        }
        else {
            if(frameIndex == 2) hitboxManager->addHurtbox(x + 10, y, mirrored,
                                                          sideSpecial1, player);
            else if(frameIndex == 3) hitboxManager->addHurtbox(x + 10, y, mirrored,
                                                               sideSpecial2, player);
            else if(frameIndex == 4) hitboxManager->addHurtbox(x + 10, y, mirrored,
                                                               sideSpecial3, player);
            else if(frameIndex == 5) hitboxManager->addHurtbox(x + 10, y, mirrored,
                                                               sideSpecial4, player);
        }
    }
    else if(action == VAL_ACTION_DOWNSPECIAL) {
        animationIndex = 34;
        mirrored = l_mirrored;
        disabledFrames = 2;

        xAnimationOffset = 0;
        yAnimationOffset = 0;
        x_mirroredOffset = -6;

        if(frameIndex == 0) frameExtension = 3;
        else if(frameIndex == 1) frameExtension = 8;
        if(frameLengthCounter++ >= frameExtension) {
            frameLengthCounter = 0;
            frameIndex++;

            if(frameIndex == 1) {
                //  release robot car
                float robotCarXAnimationOffset = 0;
                float robotCarXMirroredAnimationOffset = 0;
                float robotCarYAnimationOffset = 0;

                if(mirrored) robotCarXAnimationOffset = 0;
                else robotCarXMirroredAnimationOffset = 0;

                robotCarActive = true;
                robotCarActivationFlag = false;
                robotCarStartTime = currentTime;
                if(mirrored) robotX = x + x_mirroredOffset + robotCarXMirroredAnimationOffset;
                else robotX = x + xAnimationOffset + robotCarXAnimationOffset;
                robotY = y + yAnimationOffset + robotCarYAnimationOffset;
                robotMirrored = mirrored;
                robotCarFrameIndex = 0;
            }
        }
        if(frameIndex >= 2) {
            disabledFrames = 1;
            if(y == floor) action = VAL_ACTION_RESTING;
            else action = VAL_ACTION_FALLING;
        }

    }
    else if(action == VAL_ACTION_NEUTRALSPECIAL) {
        animationIndex = 30;
        mirrored = l_mirrored;
        disabledFrames = 2;

        xAnimationOffset = 0;
        yAnimationOffset = 0;
        x_mirroredOffset = -14;

        hitbox.offsetY(-1);
        hitbox.offsetHeight(-2);
        hitbox.offsetWidth(2);

        frameExtension = 2;
        if(frameIndex == 2) frameExtension = 5;
        if(frameLengthCounter++ >= frameExtension) {
            frameLengthCounter = 0;
            frameIndex++;

            if(frameIndex == 2) {
                int xLaserOffset = 30;
                int yLaserOffset = 14;
                int xMirroredLaserOffset = -10;

                if(mirrored) xLaserOffset = 0;
                else xMirroredLaserOffset = 0;

                laserActive = true;
                laserActivationFlag = false;
                laserStartTime = currentTime;
                if(mirrored) laserX = x + x_mirroredOffset + xMirroredLaserOffset;
                else laserX = x + xAnimationOffset + xLaserOffset;
                laserY = y + yAnimationOffset + yLaserOffset;
                laserMirrored = mirrored;
            }
        }

        if(frameIndex >= 3) {
            disabledFrames = 4;
            if (y == floor) action = VAL_ACTION_RESTING;
            else action = VAL_ACTION_FALLING;
        }
    }
    else if(action == VAL_ACTION_SHIELD) {
        animationIndex = 41;
        frameIndex = 0;
        mirrored = l_mirrored;

        if(y > floor) x += airSpeed * 0.3 * joyH;

        xAnimationOffset = 1;
        yAnimationOffset = 0;
        x_mirroredOffset = -1;

        int xShieldOffset = xAnimationOffset + 3;
        int yShieldOffset = yAnimationOffset + 3;
        int x_mirroredShieldOffset = x_mirroredOffset + 3;

        disabledFrames = 2;
        invulnerableFrames = 2;

        shieldDamage += PLAYER_SHIELD_DEGEN;

        if(currentTime - l_shieldFall_t == 0) {
            if(y == floor) action = VAL_ACTION_RESTING;
            else action = VAL_ACTION_FALLING;
        }
        else if(shieldDamage > PLAYER_SHIELD_MAXDAMAGE) {
            action = VAL_ACTION_STUN;
            frameLengthCounter = 0;
            frameIndex = 0;
            stunTimeStart = currentTime;
        }
        else {
            uint8_t shieldIndex;
            if(shieldDamage < PLAYER_SHIELD_MAXDAMAGE / 3.) shieldIndex = 0;
            else if(shieldDamage < (PLAYER_SHIELD_MAXDAMAGE * 2.) / 3.) shieldIndex = 1;
            else shieldIndex = 2;

            if(player == 2) shieldIndex += 3;

            if(mirrored) xShieldOffset = 0;
            else x_mirroredShieldOffset = 0;

            SpriteSendable shield;
            //  animate shield
            shield.charIndex = 3;
            shield.animationIndex = 11;
            shield.frame = shieldIndex;
            shield.framePeriod = 1;
            shield.persistent = false;
            shield.continuous = false;
            shield.x = (int16_t) x + x_mirroredShieldOffset + xShieldOffset;
            shield.y = (int16_t) y + yShieldOffset;
            shield.layer = LAYER_NAMETAG;
            shield.mirrored = mirrored;

            UART_sendAnimation(shield);
        }
    }
    else if(action == VAL_ACTION_HURT) {
        animationIndex = 40;

        xAnimationOffset = 7;
        yAnimationOffset = -2;
        x_mirroredOffset = 5;

        if(disabledFrames == -1) {
            //  knockback
            x += DIKnockbackHorizontalSpeed * joyH;
            y += DIKnockbackVerticalSpeed * joyV;
        }
        else {
            x += DIHorizontalSpeed * joyH;
            y += DIVerticalSpeed * joyV;
        }

        frameExtension = 2;
        if (frameLengthCounter++ >= frameExtension) {
            frameLengthCounter = 0;
            frameIndex++;
        }
        frameIndex %= 2;
    }
    if(action == VAL_ACTION_STUN) {
        animationIndex = 43;
        disabledFrames = 2;

        xAnimationOffset = 0;
        yAnimationOffset = 0;
        x_mirroredOffset = 3;

        if(currentTime - stunTimeStart >= PLAYER_STUN_LENGTH_SECONDS * 1000)  {
            if(y == floor) action = VAL_ACTION_RESTING;
            else action = VAL_ACTION_FALLING;
        }

        frameExtension = 7;
        if(frameLengthCounter++ >= frameExtension) {
            frameLengthCounter = 0;
            frameIndex++;
            frameIndex %= 4;
        }
    }
    if(action == VAL_ACTION_CROUCHING) {

        animationIndex = 1;
        frameIndex = 0;

        x_mirroredOffset = -3;

        hitbox.offsetY(-1);
        hitbox.offsetHeight(-2);
        hitbox.offsetWidth(2);

        if(onPlatform) {
            action = VAL_ACTION_FALLING;
            y -= 1;
            yVel = 0;
            floor = stage->floor(x + VAL_STAGE_OFFSET, y);
        }

        if(joyV > -0.3) {
            action = VAL_ACTION_RESTING;
            lastBlink = currentTime;
        }
    }
    if(action == VAL_ACTION_FALLING) {
        if(y <= floor) {
            y = floor;
            yVel = 0;
            l_action = VAL_ACTION_FALLING;
            if(joyH == 0) {
                action = VAL_ACTION_RESTING;
                lastBlink = currentTime;
            } else {
                action = VAL_ACTION_RUNNING;
            }
        }
        else {
            x_mirroredOffset = 0;
            xAnimationOffset = -3;
            yAnimationOffset = -5;
            x += airSpeed * joyH;
            if(x > rightBound) x = rightBound;
            else if(x < leftBound) x = leftBound;

            gravityScale = 1;

            mirrored = l_mirrored;
            hitbox.offsetWidth(4);
            hitbox.offsetY(-1);
            if(mirrored) hitbox.offsetX(4);
            else hitbox.offsetX(2);
            animationIndex = 4;

            frameExtension = 3;
            if (frameLengthCounter++ >= frameExtension) {
                frameLengthCounter = 0;
                frameIndex++;
            }
            frameIndex %= 2;
        }
    }
    if(action == VAL_ACTION_RESTING) {
        //  standing, resting

        //  mirrored facing left/right
        mirrored = l_mirrored;

        frameExtension = 0;
        animationIndex = 0;
        x_mirroredOffset = 0;
        xAnimationOffset = 0;
        yAnimationOffset = 0;

        hitbox.offsetHeight(0);
        hitbox.offsetWidth(0);
        hitbox.offsetY(0);
        if(mirrored) hitbox.offsetX(2);
        else hitbox.offsetX(0);

        if (l_action != VAL_ACTION_RESTING || lastBlink == 0) {
            lastBlink = currentTime;
        }

        if (currentTime - lastBlink > blinkPeriod) {
            if (frameLengthCounter++ >= frameExtension) {
                frameLengthCounter = 0;
                frameIndex++;
            }
            if (frameIndex >= 4) {
                frameIndex = 0;
                lastBlink = currentTime;
            }
        } else frameIndex = 0;
    }
    if(action == VAL_ACTION_RUNNING) {
        if(y != floor) {
            action = VAL_ACTION_FALLING;
        }
        else if(joyH == 0) {
            action = VAL_ACTION_RESTING;
        }
        else {
            //  x position
            x += joyH * groundSpeed;
            if(x > rightBound) x = rightBound;
            else if(x < leftBound) x = leftBound;

            //  mirrored facing left/right
            if (joyH == 0) mirrored = l_mirrored;
            else mirrored = joyH < 0;

            animationIndex = 2;
            int8_t calculatedExtension = -5*absVal(joyH) + 3.5;
            if(calculatedExtension < 0) frameExtension = 0;
            else if(calculatedExtension > 2) frameExtension = 2;
            else frameExtension = calculatedExtension;
            if (frameLengthCounter++ > frameExtension) {
                frameLengthCounter = 0;
                frameIndex++;
            }
            frameIndex %= 5;

            xAnimationOffset = -1;
            yAnimationOffset = 0;
            x_mirroredOffset = 1;
        }
    }

    l_action = action;

    //  laser projectile
    if(laserActive) {
        SpriteSendable laser;
        laser.charIndex = charIndex;
        laser.frame = 0;
        laser.mirrored = laserMirrored;
        laser.animationIndex = 33;
        laser.x = laserX;
        laser.y = laserY;
        laser.continuous = false;
        laser.persistent = false;
        laser.layer = LAYER_CHARACTER_PROJECTILE;
        laser.framePeriod = 1;

        UART_sendAnimation(laser);

        hitboxManager->addHurtbox(laserX+7, laserY, laserMirrored,
                                  laserProjectile, player);

        if(currentTime - laserStartTime > 500 || laserActivationFlag) {
            laserActive = false;
        }
        else {
            //  update laser position
            double laserSpeed = 7.0;
            if(laserMirrored) laserX -= laserSpeed;
            else laserX += laserSpeed;
        }
    }

    //  robot car
    if(robotCarActive) {
        SpriteSendable robot;
        robot.charIndex = charIndex;
        robot.frame = robotCarFrameIndex;
        robot.mirrored = robotMirrored;
        robot.animationIndex = 35;
        robot.x = robotX;
        robot.y = robotY;
        robot.continuous = false;
        robot.persistent = false;
        robot.layer = LAYER_CHARACTER_PROJECTILE;
        robot.framePeriod = 1;

        UART_sendAnimation(robot);

        if(currentTime - robotCarStartTime > 3000 || robotCarActivationFlag) {
            //  explode
            if(robotCarFrameIndex < 2) robotCarFrameIndex = 2;

            frameExtension = 2;
            if(robotCarFrameCounter++ >= frameExtension) {
                robotCarFrameIndex++;
            }
            if(robotCarFrameIndex >= 5) {
                robotCarActive = false;
            }
        }
        else {
            (++robotCarFrameIndex) %= 2;

            hitboxManager->addHurtbox(robotX + 9, robotY, mirrored,
                                      robotCar, player);

            if (robotY < -5) robotCarActive = false;
            else {
                //  update robot car position
                float robotFloor = stage->floor(robotX + 7, robotY);
                double robotSpeed = 2.0;
                if (robotMirrored) robotX -= robotSpeed;
                else robotX += robotSpeed;

                //  gravity
                robotYVel -= gravityFalling;
                if (robotYVel < -5) robotYVel = -5;
                robotY += robotYVel;
                if (robotY < robotFloor) robotY = robotFloor;
            }
        }
    }

    //  regenerate shield
    if(action != VAL_ACTION_SHIELD) {
        shieldDamage -= PLAYER_SHIELD_REGEN;
        if(shieldDamage < 0) shieldDamage = 0;
    }

    //  disabled means can interrupt current action and start new action
    if(disabledFrames > 0) disabledFrames--;
    if(invulnerableFrames > 0) invulnerableFrames--;
    if(disabledFrames == -1) {
        //  knockback stun, remove stun when falling or on floor
        if(y == floor || yVel < 0) {
            if(y > floor) action = VAL_ACTION_FALLING;
            disabledFrames = 0;
        }
    }

    if(x > rightBound) x = rightBound;
    else if(x < leftBound) x = leftBound;

    this->hitbox.initialize(x+8, y+17, 18, 34);

    if(!mirrored) x_mirroredOffset = 0;
    else x_mirroredOffset -= xAnimationOffset;

    s.charIndex = charIndex;
    s.animationIndex = animationIndex;
    s.framePeriod = 1;
    s.frame = frameIndex;
    s.persistent = false;
    s.continuous = false;
    s.x = (int16_t) x + x_mirroredOffset + xAnimationOffset;
    s.y = (int16_t) y + yAnimationOffset;
    s.layer = LAYER_CHARACTER;
    s.mirrored = mirrored;

    UART_sendAnimation(s);

    //  update velocity and positions
    if(yVel < maxFallingVelocity) yVel = maxFallingVelocity;

    y += yVel;
    if(y > ceiling && action != VAL_ACTION_LEDGEGRAB) y = ceiling;
    if(y <= floor) {
        y = floor;
        yVel = 0;
        jumpsUsed = 0;
    }

    if(yVel > 0) yVel -= gravityRising * gravityScale;
    else yVel -= gravityFalling * gravityScale;

    if(maxHorizontalSpeed < absVal(xVel)) {
        if(xVel < 0) xVel = -maxHorizontalSpeed;
        else xVel = maxHorizontalSpeed;
    }
    if(xVel != 0) {
        if(x == floor) xVel *= groundFriction;

        if(absVal(xVel) < airResistance) xVel = 0;

        else if(xVel > 0) xVel -= airResistance;
        else if(xVel < 0) xVel += airResistance;
    }
    x += xVel + stageVelocity;

    if(y == floor || action == GAW_ACTION_LEDGEGRAB) noJumpsDisabled = false;

    //  start any new sequences
    //  neutral attack
    if(disabledFrames == 0 && absVal(joyH) < 0.15 && absVal(joyV) < 0.15 &&
       (action == VAL_ACTION_RESTING) && currentTime - l_btnARise_t == 0) {
        action = VAL_ACTION_JAB;
        disabledFrames = 2;
        frameIndex = 0;
        frameLengthCounter = 0;

    }
//        //  down tilt
//    else if(disabledFrames == 0 && y == floor && (VAL_ACTION_CROUCHING || action == VAL_ACTION_RESTING)
//            && currentTime - l_btnARise_t == 0 && joyV < -0.3) {
//        action = VAL_ACTION_DOWNTILT;
//        disabledFrames = 2;
//        frameIndex = 0;
//        frameLengthCounter = 0;
//    }
        //  neutral air
    else if(disabledFrames == 0 && y > floor
            && (action == VAL_ACTION_FALLING || action == VAL_ACTION_JUMPING
                || action == VAL_ACTION_DOUBLEJUMPING)
            && currentTime - l_btnARise_t == 0
            && absVal(joyH) < 0.3 && absVal(joyV) < 0.3) {
        action = VAL_ACTION_NEUTRALAIR;
        mirrored = l_mirrored;
        disabledFrames = 2;
        frameIndex = 0;
        frameLengthCounter = 0;
    }
        //  down air
    else if(disabledFrames == 0 && y > floor
            && (action == VAL_ACTION_FALLING || action == VAL_ACTION_JUMPING
                || action == VAL_ACTION_DOUBLEJUMPING)
            && currentTime - l_btnARise_t == 0 && joyV < -0.3) {
        action = VAL_ACTION_DOWNAIR;
        if(joyH == 0) mirrored = l_mirrored;
        else mirrored = joyH < 0;
        disabledFrames = 2;
        frameIndex = 0;
        frameLengthCounter = 0;
    }
//        //  up air
//    else if(disabledFrames == 0 && y > floor
//            && (action == VAL_ACTION_FALLING || action == VAL_ACTION_JUMPING
//                || action == VAL_ACTION_DOUBLEJUMPING)
//            && currentTime - l_btnARise_t == 0 && joyV > 0.3) {
//        action = VAL_ACTION_UPAIR;
//        if(joyH == 0) mirrored = l_mirrored;
//        else mirrored = joyH < 0;
//        disabledFrames = 2;
//        frameIndex = 0;
//        frameLengthCounter = 0;
//    }
        //  forward + back air
    else if(disabledFrames == 0 && y > floor
            && (action == VAL_ACTION_FALLING || action == VAL_ACTION_JUMPING
                || action == VAL_ACTION_DOUBLEJUMPING)
            && currentTime - l_btnARise_t == 0 && absVal(joyH) > 0.3) {
        action = VAL_ACTION_FORWARDAIR;
        mirrored = joyH < 0;
        disabledFrames = 2;
        frameIndex = 0;
        frameLengthCounter = 0;
    }
//        //  forward tilt
//    else if(disabledFrames == 0 && y == floor && (action == VAL_ACTION_RUNNING || action == VAL_ACTION_RESTING)
//            && currentTime - l_btnARise_t == 0 && absVal(joyH) < 0.6 && absVal(joyH) > 0.1) {
//        action = VAL_ACTION_FORWARDTILT;
//        mirrored = joyH < 0;
//        disabledFrames = 2;
//        frameIndex = 0;
//        frameLengthCounter = 0;
//    }
//        //  up tilt
//    else if(disabledFrames == 0 && y == floor && (action == VAL_ACTION_RUNNING || action == VAL_ACTION_RESTING)
//            && currentTime - l_btnARise_t == 0 && joyV > 0) {
//        action = VAL_ACTION_UPTILT;
//        disabledFrames = 2;
//        frameIndex = 0;
//        frameLengthCounter = 0;
//    }
        //  dash attack
    else if(disabledFrames == 0 && y == floor && action == VAL_ACTION_RUNNING
            && currentTime - l_btnARise_t == 0 && absVal(joyH) > 0.4) {
        action = VAL_ACTION_DASHATTACK;
        mirrored = joyH < 0;
        disabledFrames = 2;
        frameIndex = 0;
        frameLengthCounter = 0;
        dashAttackStartTime = currentTime;
    }
        //  down B
    else if(!robotCarActive && disabledFrames == 0 && currentTime - l_btnBRise_t == 0
            && joyV < -0.5) {
        action = VAL_ACTION_DOWNSPECIAL;
        disabledFrames = 2;
        frameIndex = 0;
        frameLengthCounter = 0;
        mirrored = l_mirrored;
    }
        //  up special
    else if(!noJumpsDisabled &&
            ( (action == VAL_ACTION_FALLING || action == VAL_ACTION_JUMPING  ||
               action == VAL_ACTION_DOUBLEJUMPING) ||
              (y == floor && (action == VAL_ACTION_RESTING || action == VAL_ACTION_RUNNING)) ) &&
            currentTime-l_btnBRise_t == 0 && joyV > 0.3) {
        action = VAL_ACTION_UPSPECIAL;
        if(joyH == 0) mirrored = l_mirrored;
        else mirrored = joyH < 0;
        disabledFrames = 2;
        frameIndex = 0;
        frameLengthCounter = 0;
        if(yVel < -3) yVel = -3;
        else if(yVel > 0) yVel = 0;

        upSpecialStartTime = currentTime;
        y++;
    }
        //  side special
    else if(disabledFrames == 0 && currentTime - l_btnBRise_t == 0
            && absVal(joyH) >= 0.5) {
        action = VAL_ACTION_SIDESPECIAL;
        disabledFrames = 2;
        frameIndex = 0;
        frameLengthCounter = 0;
        mirrored = joyH < 0;
    }
        //  neutral B
    else if(disabledFrames == 0 && currentTime - l_btnBRise_t == 0
            && absVal(joyH) < 0.5 && !laserActive) {
        action = VAL_ACTION_NEUTRALSPECIAL;
        disabledFrames = 2;
        frameIndex = 0;
        frameLengthCounter = 0;
        mirrored = l_mirrored;
    }

        //  movement
        //  jumping
    else if((disabledFrames == 0 &&
             (action == VAL_ACTION_RESTING || action == VAL_ACTION_CROUCHING || action == VAL_ACTION_RUNNING
              || action == VAL_ACTION_HURT)
             && (joyV - l_joyV) > joystickJumpSpeed && l_joyV > -0.1 && y == floor)
            || (action == VAL_ACTION_LEDGEGRAB && (joyV - l_joyV) > joystickJumpSpeed && l_joyV > -0.1
                && disabledFrames == 0)) {
        jumpsUsed = 0;
        disabledFrames = 1;
        yVel = initialJumpSpeed;
        l_action = action;
        action = VAL_ACTION_JUMPING;
        frameIndex = 0;
        frameLengthCounter = 0;
        ledgeGrabTime = currentTime;
    }
        //  double jump
    else if( disabledFrames == 0
             && ( (action == VAL_ACTION_JUMPING || action == VAL_ACTION_FALLING
                   || action == VAL_ACTION_DOUBLEJUMPING
                   || (action == VAL_ACTION_FORWARDAIR || action == VAL_ACTION_UPAIR) )
                  && jumpsUsed == 0 && (joyV - l_joyV) > joystickJumpSpeed && l_joyV > -0.1) ) {
        jumpsUsed++;
        yVel = repeatedJumpSpeed;
        l_action = action;
        action = VAL_ACTION_DOUBLEJUMPING;
        frameIndex = 0;
        frameLengthCounter = 0;

        if(joyH == 0) mirrored = l_mirrored;
        else mirrored = joyH < 0;
    }
//        //  shield
//    else if(disabledFrames == 0 &&
//            ( (action == VAL_ACTION_FALLING || action == VAL_ACTION_JUMPING  ||
//               action == VAL_ACTION_DOUBLEJUMPING) ||
//              (y == floor && (action == VAL_ACTION_RESTING || action == VAL_ACTION_RUNNING ||
//                              action == VAL_ACTION_CROUCHING)) )
//            && shield && !l_shield && (PLAYER_SHIELD_MAXDAMAGE - shieldDamage > 10)) {
//        action = VAL_ACTION_SHIELD;
//        disabledFrames = 2;
//    }
        //  running/walking
    else if(((action == VAL_ACTION_RESTING) || (disabledFrames == 0 && action == VAL_ACTION_HURT))
            && absVal(joyH) > 0) {
        l_action = action;
        action = VAL_ACTION_RUNNING;
        frameIndex = 0;
        frameLengthCounter = 0;
    }
        //  crouching
    else if((action == VAL_ACTION_RESTING || action == VAL_ACTION_RUNNING || action == VAL_ACTION_HURT) &&
            joyV <= -0.3 && y == floor) {
        l_action = action;
        action = VAL_ACTION_CROUCHING;

        frameIndex = 0;
        frameLengthCounter = 0;
    }
        //  resting
    else if(disabledFrames == 0 &&
            joyH == 0 && joyV == 0 && (action == VAL_ACTION_FALLING || action == VAL_ACTION_HURT)) {
        l_action = action;
        if(y == floor) action = VAL_ACTION_RESTING;
        else action = VAL_ACTION_FALLING;

        frameIndex = 0;
        frameLengthCounter = 0;
    }
        //  ledge grab drop down
    else if(disabledFrames == 0 && (action == VAL_ACTION_LEDGEGRAB) &&
            joyV < -0.3) {
        action = VAL_ACTION_FALLING;
        y -= 30;
        ledgeGrabTime = currentTime;
    }

    updateLastValues(joyH, joyV, btnA, btnB, shield);
}

void Valvano::updateLastValues(float joyH, float joyV, bool btnA, bool btnB, bool shield) {
    l_joyH = joyH;
    l_joyV = joyV;
    l_btnA = btnA;
    l_btnB = btnB;
    l_shield = shield;

    l_mirrored = mirrored;
}

void Valvano::collide(Hurtbox *hurtbox, Player *otherPlayer) {
    //  ledge grab
    if(hurtbox->source == 0) {
        if(this->hitbox.y < hurtbox->y
           && currentTime - ledgeGrabTime > 1000
           && yVel <= 0 && action != VAL_ACTION_SHIELD) {
            action = VAL_ACTION_LEDGEGRAB;
            mirrored = hurtbox->damage != 0;
            yVel = 0;
            xVel = 0;
            x = hurtbox->x;
            y = hurtbox->y;
            disabledFrames = 6;
            jumpsUsed = 0;
        }
        return;
    }
    else if(action == VAL_ACTION_SHIELD) {
        if(hurtbox->damage < PLAYER_SHIELD_MAXDAMAGE/2.) shieldDamage += hurtbox->damage * 0.3;
        else shieldDamage += PLAYER_SHIELD_MAXDAMAGE/2.;

        if(hurtbox->activationFlagPointer != nullptr) {
            *(hurtbox->activationFlagPointer) = true;
        }
    }
        // only knockback if not currently knocked back
    else if(disabledFrames != -1 && invulnerableFrames == 0) {
        disabledFrames = hurtbox->stunFrames;
        damage += hurtbox->damage;

        float knockbackMultiplier = damage / 200. + 1.0;
//        printf("%0.1f\n", damage);

        if (otherPlayer->x < x) xVel = hurtbox->xKnockback * knockbackMultiplier;
        else xVel = -hurtbox->xKnockback * knockbackMultiplier;
        yVel = hurtbox->yKnockback * knockbackMultiplier;

        action = VAL_ACTION_HURT;
    }
}

void Valvano::reset() {
    action = VAL_ACTION_RESTING;
    yVel = 0;
    xVel = 0;
    currentTime = 0;
    damage = 0;
    action = 0;
    l_action = 0;
    frameIndex = 0;
    frameExtension = 1;
    frameLengthCounter = 0;
    deathTime = 0;
    disabledFrames = 0;
    invulnerableFrames = 0;
    noJumpsDisabled = false;

    l_joyV = 0;
    l_joyH = 0;
    l_btnA = false;
    l_btnARise_t = -1;
    l_btnAFall_t = -1;
    l_btnB = false;
    l_btnBRise_t = -1;
    l_btnBFall_t = -1;
    l_shield = false;
    l_shieldRise_t = -1;
    l_shieldFall_t = -1;

    fsmash_startTime = 0;
    dsmash_startTime = 0;
    usmash_startTime = 0;
    ledgeGrabTime = 0;
    jumpsUsed = 0;

    laserActive = false;
    robotCarActive = false;

    shieldDamage = 0;
    dead = false;
}


