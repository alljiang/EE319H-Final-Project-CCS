//
// Created by Allen on 4/21/2020.
//

#include "entities.h"
#include "utils.h"
#include "metadata.h"
#include "UART.h"
#include "stage.h"
#include "Audio.h"

void GameandWatch::controlLoop(float joyH, float joyV, bool btnA, bool btnB, bool shield,
                        class Stage *stage, class HitboxManager *hitboxManager) {
    //  assume joystick deadzone filtering is already done

    float dt = 49;
    currentTime += (uint8_t)dt;

    SpriteSendable s;

    //  check if dead
    if(dead) {
        if(deathTime == 0) {
            deathTime = currentTime;

            Audio_destroy(&audio1);
            Audio_play(SOUND_DEATHBLAST, 0.5, &audio1);
        }
        else if(currentTime - deathTime > 1000) {
            //  respawn

            //  reset
            reset();

            y = 240;
            x = stage->getStartX(player);
            invulnerableFrames = 20 * 3;

            dead = false;
            action = GAW_ACTION_FALLING;

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

    bool continuous = false;

    hitbox.offsetY(0);
    hitbox.offsetX(0);
    hitbox.offsetRadius(0);
    hitbox.offsetWidth(0);
    hitbox.offsetHeight(0);

    float ceiling = stage->ceil(x + GAW_STAGE_OFFSET, y);
    float floor = stage->floor(x + GAW_STAGE_OFFSET, y);
    float leftBound = stage->leftBound(x + GAW_STAGE_OFFSET, y) - GAW_STAGE_OFFSET / 2;
    float rightBound = stage->rightBound(x - GAW_STAGE_OFFSET, y) - GAW_STAGE_OFFSET;
    bool onPlatform = stage->onPlatform(x + KIRBY_STAGE_OFFSET, y);
    float stageVelocity = stage->xVelocity(x, y);
    float gravityScale = 1;

    //  first, follow up on any currently performing actions

    //  movement
    if(action == GAW_ACTION_JUMPING) {
        hitbox.offsetY(0);
        if(mirrored) hitbox.offsetX(4);
        else hitbox.offsetX(2);
        gravityScale = 1;
        x += airSpeed * joyH;
        if(x > rightBound) x = rightBound;
        else if(x < leftBound) x = leftBound;

        mirrored = l_mirrored;
        animationIndex = 3;

        frameExtension = 3;
        if(frameLengthCounter++ >= frameExtension) {
            frameLengthCounter = 0;
            frameIndex++;
        }
        if(frameIndex >= 4) {
            l_action = GAW_ACTION_JUMPING;
            action = GAW_ACTION_FALLING;
        }
    }
    else if(action == GAW_ACTION_DOUBLEJUMPING) {
        gravityScale = 1;
        x += airSpeed * joyH;
        //  give bonus speed for fighting knockback
        if((xVel < 0 && joyH > 0) ||  (xVel > 0 && joyH < 0)) { x+= airSpeed * joyH; }
        if(x > rightBound) x = rightBound;
        else if(x < leftBound) x = leftBound;
        animationIndex = 4;

        hitbox.offsetY(0);
        if(mirrored) hitbox.offsetX(3);
        else hitbox.offsetX(1);

        frameExtension = 3;
        if(frameLengthCounter++ >= frameExtension) {
            frameLengthCounter = 0;
            frameIndex++;
        }
        if(frameIndex >= 4) {
            l_action = GAW_ACTION_DOUBLEJUMPING;
            action = GAW_ACTION_FALLING;
        }
    }
    else if(action == GAW_ACTION_LEDGEGRAB) {
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
    else if(action == GAW_ACTION_NEUTRALATTACK) {
        animationIndex = 17;
        mirrored = l_mirrored;
        x_mirroredOffset = -22;

        disabledFrames = 2;
        frameExtension = 2;

        hitbox.offsetY(0);
        if(mirrored) hitbox.offsetX(5);
        else hitbox.offsetX(0);

        if(frameLengthCounter++ >= frameExtension) {
            frameLengthCounter = 0;
            frameIndex++;
        }
        if(frameIndex >= 2) {
            frameIndex = 0;
            frameLengthCounter = 0;
        }
        if(currentTime-l_btnARise_t > 300) {
            l_action = GAW_ACTION_NEUTRALATTACK;
            x_mirroredOffset = 0;
            action = GAW_ACTION_RESTING;

            disabledFrames = 2;
        }

        if(frameIndex == 1) {
            hitboxManager->addHurtbox(x + 17, y, mirrored,
                    neutralAttack, player);

            Audio_destroy(&audio2);
            Audio_play(GAW_SOUND_NEUTRALATTACK, 0.5, &audio2);
        }
    }
    else if(action == GAW_ACTION_DASHATTACK) {
        animationIndex = 13;
        mirrored = l_mirrored;
        x_mirroredOffset = 0;
        xAnimationOffset = 0;
        disabledFrames = 2;
        frameIndex = 0;

        float baseSpeed = 4;
        frameExtension = 3;
        if(frameLengthCounter++ >= frameExtension) {
            frameLengthCounter = 0;
            if(!mirrored) x += baseSpeed * (1 + frameExtension);
            else x -= baseSpeed * (1 + frameExtension);
        }

        gravityScale = 0;
        yVel = 0;

        hitbox.offsetWidth(6);
        hitbox.offsetY(0);
        hitbox.offsetX(3);

        if(currentTime - dashAttackStartTime > 700) {
            l_action = GAW_ACTION_DASHATTACK;
            if(absVal(joyH) > 0.1) action = GAW_ACTION_RUNNING;
            else action = GAW_ACTION_RESTING;
            disabledFrames = 4;
        }
        else {
            hitboxManager->addHurtbox(x + 18, y, mirrored,
                                      dashAttack, player);
        }
    }
    else if(action == GAW_ACTION_DOWNTILT) {
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
            l_action = GAW_ACTION_DOWNTILT;
            if(joyV < -0.25) {
                action = GAW_ACTION_CROUCHING;
            }
            else action = GAW_ACTION_RESTING;
            frameIndex = 0;
            disabledFrames = 4;
            x_mirroredOffset = 0;
        }
        else {
            hitboxManager->addHurtbox(x + 18, y, mirrored,
                                      downTilt, player);
        }
    }
    else if(action == GAW_ACTION_FORWARDTILT) {
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
            l_action = GAW_ACTION_FORWARDTILT;
            action = GAW_ACTION_RESTING;
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
    else if(action == GAW_ACTION_UPTILT) {
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
            l_action = GAW_ACTION_UPTILT;
            action = GAW_ACTION_RESTING;
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
    else if(action == GAW_ACTION_FORWARDSMASHHOLD) {
        animationIndex = 14;
        mirrored = l_mirrored;
        x_mirroredOffset = -20;

        hitbox.offsetWidth(9);
        hitbox.offsetHeight(2);
        hitbox.offsetY(0);
        if(mirrored) hitbox.offsetX(-2);
        else hitbox.offsetX(4);

        //  release attack
        if((!btnA && currentTime - fsmash_startTime > 300) || currentTime - fsmash_startTime > 3000) {
            action = GAW_ACTION_FORWARDSMASH;
            frameIndex = 2;

            Audio_destroy(&audio2);
            Audio_play(GAW_SOUND_SMASHATTACK, 0.5, &audio2);
        }
            //  charging attack
        else {
            disabledFrames = 2;
            frameExtension = 2;
            if (frameLengthCounter++ >= frameExtension) {
                frameLengthCounter = 0;
                frameIndex++;
            }
            if(frameIndex >= 2) {
                frameIndex = 0;
            }
        }
    }
    else if(action == GAW_ACTION_FORWARDSMASH) {
        animationIndex = 14;
        mirrored = l_mirrored;
        x_mirroredOffset = -20;

        hitbox.offsetWidth(9);
        hitbox.offsetHeight(2);
        hitbox.offsetY(0);
        if(mirrored) hitbox.offsetX(-2);
        else hitbox.offsetX(4);

        disabledFrames = 2;
        frameExtension = 6;
        if (frameLengthCounter++ >= frameExtension) {
            frameLengthCounter = 0;
            frameIndex++;
        }
        if(frameIndex >= 4) {
            l_action = GAW_ACTION_FORWARDSMASH;
            action = GAW_ACTION_RESTING;
            disabledFrames = 5;
        }
        else {
            float chargeScale = (currentTime - fsmash_startTime) / 3000. * 0.6 + 1;
            hitboxManager->addHurtbox(x + 15, y, mirrored,
                                      forwardSmash, player, chargeScale);
        }
    }
    else if(action == GAW_ACTION_UPSMASHHOLD) {
        animationIndex = 16;
        mirrored = l_mirrored;
        x_mirroredOffset = -10;
        xAnimationOffset = -12;

        hitbox.offsetWidth(7);
        if(mirrored) hitbox.offsetX(7);
        else hitbox.offsetX(-9);

        //  release attack
        if((!btnA && currentTime - usmash_startTime > 300) || currentTime - usmash_startTime > 3000) {
            action = GAW_ACTION_UPSMASH;
            frameIndex = 2;

            Audio_destroy(&audio2);
            Audio_play(GAW_SOUND_SMASHATTACK, 0.5, &audio2);
        }
            //  charging attack
        else {
            disabledFrames = 2;
            frameExtension = 2;
            if (frameLengthCounter++ >= frameExtension) {
                frameLengthCounter = 0;
                frameIndex++;
            }
            if(frameIndex >= 2) {
                frameIndex = 0;
            }
        }
    }
    else if(action == GAW_ACTION_UPSMASH) {
        animationIndex = 16;
        mirrored = l_mirrored;
        x_mirroredOffset = -10;
        xAnimationOffset = -12;

        disabledFrames = 2;
        frameExtension = 4;
        if(frameIndex == 3) frameExtension = 8;
        if (frameLengthCounter++ >= frameExtension) {
            frameLengthCounter = 0;
            frameIndex++;
        }
        if(frameIndex >= 4) {
            l_action = GAW_ACTION_UPSMASH;
            action = GAW_ACTION_RESTING;
            disabledFrames = 2;
        }
        else {
            float chargeScale = (currentTime - usmash_startTime) / 3000. * 0.6 + 1;
            if(frameIndex > 1) {
                hitboxManager->addHurtbox(x + 14, y, mirrored,
                                          upSmash, player, chargeScale);
            }
        }
    }
    else if(action == GAW_ACTION_DOWNSMASHHOLD) {
        animationIndex = 15;
        mirrored = l_mirrored;
        x_mirroredOffset = -17;
        xAnimationOffset = -17;

        hitbox.offsetHeight(2);
        hitbox.offsetY(0);
        if(mirrored) hitbox.offsetX(2);
        else hitbox.offsetX(0);

        //  release attack
        if((!btnA && currentTime - dsmash_startTime > 300) || currentTime - dsmash_startTime > 3000) {
            action = GAW_ACTION_DOWNSMASH;
            frameIndex = 2;

            Audio_destroy(&audio2);
            Audio_play(GAW_SOUND_SMASHATTACK, 0.5, &audio2);
        }
            //  charging attack
        else {
            disabledFrames = 2;
            frameExtension = 2;
            if (frameLengthCounter++ >= frameExtension) {
                frameLengthCounter = 0;
                frameIndex++;
            }
            if(frameIndex >= 2) {
                frameIndex = 0;
            }
        }
    }
    else if(action == GAW_ACTION_DOWNSMASH) {
        animationIndex = 15;
        mirrored = l_mirrored;
        x_mirroredOffset = -17;
        xAnimationOffset = -17;

        hitbox.offsetHeight(2);
        hitbox.offsetY(0);
        if(mirrored) hitbox.offsetX(2);
        else hitbox.offsetX(0);

        disabledFrames = 2;
        frameExtension = 7;
        if (frameLengthCounter++ >= frameExtension) {
            frameLengthCounter = 0;
            frameIndex++;
        }
        if(frameIndex >= 3) {
            l_action = GAW_ACTION_DOWNSMASH;
            action = GAW_ACTION_RESTING;
            disabledFrames = 5;
        }
        else {
            float chargeScale = (currentTime - dsmash_startTime) / 3000. * 0.6 + 1;
            hitboxManager->addHurtbox(x + 16, y, mirrored,
                                          downSmash, player, chargeScale);
        }
    }
    else if(action == GAW_ACTION_FORWARDAIR) {
        if(y == floor) {
            action = GAW_ACTION_RESTING;
            disabledFrames = 4;
        }
        else {
            animationIndex = 21;
            mirrored = l_mirrored;
            gravityScale = 1.0;
            x += airSpeed * joyH * 0.7;

            x_mirroredOffset = -14;
            xAnimationOffset = -1;
            yAnimationOffset = 0;

            hitbox.offsetHeight(4);
            hitbox.offsetWidth(3);
            hitbox.offsetY(3);
            if(mirrored) hitbox.offsetX(9);
            else hitbox.offsetX(4);

            disabledFrames = 2;
            if(frameIndex == 1) frameExtension = 5;
            else frameExtension = 2;
            if (frameLengthCounter++ >= frameExtension) {
                frameLengthCounter = 0;
                frameIndex++;
            }
            if (frameIndex >= 3) {
                l_action = GAW_ACTION_FORWARDAIR;
                action = GAW_ACTION_FALLING;
                disabledFrames = 4;
            }
            else {
                if(frameIndex == 1)
                    hitboxManager->addHurtbox(x + 22, y, mirrored,
                                          forwardAir, player);
            }
        }
    }
    else if(action == GAW_ACTION_BACKAIR) {
        if(y == floor) {
            action = GAW_ACTION_RESTING;
            disabledFrames = 4;
        }
        else {
            animationIndex = 22;
            mirrored = l_mirrored;
            gravityScale = 0.8;
            x += airSpeed * joyH * 0.7;

            x_mirroredOffset = 0;
            xAnimationOffset = -23;
            yAnimationOffset = 0;

            hitbox.offsetY(1);
            if(mirrored) hitbox.offsetX(2);
            else hitbox.offsetX(5);

            disabledFrames = 2;
            frameExtension = 3;
            if (frameLengthCounter++ >= frameExtension) {
                frameLengthCounter = 0;
                frameIndex++;
            }
            if (frameIndex >= 3) {
                l_action = GAW_ACTION_BACKAIR;
                action = GAW_ACTION_FALLING;
                disabledFrames = 0;
            }
            else {
                if(frameIndex == 1 || frameIndex == 2) {
                    hitboxManager->addHurtbox(x + 17, y, mirrored,
                                              backAir, player);
                }
            }
        }
    }
    else if(action == GAW_ACTION_UPAIR) {
        if(y == floor) {
            action = GAW_ACTION_RESTING;
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
                l_action = GAW_ACTION_UPAIR;
                action = GAW_ACTION_FALLING;
                disabledFrames = 5;
            }
            else {
                if(frameIndex == 1 || frameIndex == 3) {
                    hitboxManager->addHurtbox(x + 13, y, mirrored,
                                              upAir, player);

                    if(frameLengthCounter == 0) {
                        Audio_destroy(&audio2);
                        Audio_play(GAW_SOUND_UPAIR, 0.5, &audio2);
                    }
                }
            }
        }
    }
    else if(action == GAW_ACTION_DOWNAIR) {
        animationIndex = 23;
        mirrored = l_mirrored;
        x_mirroredOffset = 0;
        xAnimationOffset = 0;
        yAnimationOffset = 0;

        disabledFrames = 2;

        if(frameIndex == 0) {
            hitbox.offsetX(5);
            hitbox.offsetY(5);
            hitbox.offsetHeight(-4);

            frameExtension = 1;
            gravityScale = 0;
            yVel = 0;
            if (frameLengthCounter++ >= frameExtension) {
                frameLengthCounter = 0;
                frameIndex++;
            }
        }
        if(frameIndex == 1) {
            if(mirrored) hitbox.offsetX(11);
            else hitbox.offsetX(7);
            hitbox.offsetY(18);
            hitbox.offsetWidth(6);
            hitbox.offsetHeight(-6);

            if(y <= floor) {
                y = floor;
                frameIndex++;
                frameLengthCounter = 0;
            }
            else {
                gravityScale = 6.0;
                if(yVel < -12) yVel = -12;
                overrideMaxVelocity = true;
                xVel = 0;

                if(frameIndex == 1)
                    hitboxManager->addHurtbox(x + 22, y, mirrored,
                                              downAir, player);
            }
        }
        if(frameIndex == 2) {
            hitbox.offsetWidth(6);
            hitbox.offsetHeight(-10);
            if(mirrored) hitbox.offsetX(22);
            else hitbox.offsetX(-9);
            hitbox.offsetY(-5);

            frameExtension = 7;
            xAnimationOffset = -12;
            x_mirroredOffset = 10;
            if(frameLengthCounter++ >= frameExtension) {
                action = GAW_ACTION_RESTING;
                frameIndex++;
                disabledFrames = 1;
            }
        }
        if(frameIndex < 3) {}
        else {
            hitbox.offsetY(6);
            if(mirrored) hitbox.offsetX(2);
            else hitbox.offsetX(1);
        }
    }
    else if(action == GAW_ACTION_NEUTRALAIR) {
        if(y <= floor) {
            action = GAW_ACTION_RESTING;
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

            hitbox.offsetHeight(-3);
            hitbox.offsetWidth(4);
            hitbox.offsetY(-1);
            if(mirrored) hitbox.offsetX(4);
            else hitbox.offsetX(3);

            disabledFrames = 2;
            frameExtension = 3;
            if (frameLengthCounter++ >= frameExtension) {
                frameLengthCounter = 0;
                frameIndex++;
            }
            if (frameIndex >= 3) {
                l_action = GAW_ACTION_NEUTRALAIR;
                action = GAW_ACTION_FALLING;
                disabledFrames = 5;
            }
            else {
                hitboxManager->addHurtbox(x + 18, y, mirrored,
                                          neutralAir, player);
            }
        }
    }
    else if(action == GAW_ACTION_UPSPECIAL) {
        if(y <= floor) {
            action = GAW_ACTION_RESTING;
            disabledFrames = 3;
        }
        else {
            animationIndex = 36;
            mirrored = l_mirrored;
            x += airSpeed * joyH * 0.4;
            disabledFrames = 2;

            x_mirroredOffset = -4;
            xAnimationOffset = 0;
            yAnimationOffset = 0;

            hitbox.offsetWidth(4);
            if(mirrored) hitbox.offsetX(1);
            else hitbox.offsetX(3);
            hitbox.offsetY(0);

            if(frameIndex == 0 || frameIndex == 1) {
                frameExtension = 2;
                if(frameLengthCounter++ >= frameExtension) {
                    frameIndex++;
                    frameLengthCounter = 0;
                }

                if(frameIndex >= 2) frameIndex = 0;

                if(yVel < 3) frameIndex = 2;
            }
            if(frameIndex == 2) {
                if(yVel < -3) frameIndex = 3;
            }
            if(frameIndex == 3 || frameIndex == 4) {
                frameExtension = 2;
                if(frameLengthCounter++ >= frameExtension) {
                    frameIndex++;
                    frameLengthCounter = 0;
                }

                if(frameIndex >= 5) {
                    action = GAW_ACTION_PARACHUTE;
                    frameIndex = 0;
                    frameLengthCounter = 0;
                }
            }
        }
    }
    else if(action == GAW_ACTION_SIDESPECIAL) {
        animationIndex = 32;
        mirrored = l_mirrored;
        disabledFrames = 2;
        if(frameIndex == 0) frameExtension = 4;
        else if(frameIndex == 1) frameExtension = 5;
        else if(frameIndex == 2) frameExtension = 3;

        x_mirroredOffset = -17;
        xAnimationOffset = -11;
        yAnimationOffset = 0;

        int xNumberOffset =  18;
        int yNumberOffset = 40;
        int xMirroredNumberOffset = 6;

        if(mirrored) hitbox.offsetX(-2);
        else hitbox.offsetX(3);

        if(frameLengthCounter++ >= frameExtension) {
            frameLengthCounter = 0;
            frameIndex++;
        }
        if(frameIndex >= 3) {
            disabledFrames = 1;
            if(y == floor) action = GAW_ACTION_RESTING;
            else action = GAW_ACTION_FALLING;
        }
        else {
            if(frameIndex == 1) {
                float multiplier = 0.17857 * sideBStrength + 0.2142857;
                hitboxManager->addHurtbox(x + 17, y, mirrored,
                                          sideSpecial, player, multiplier);
            }
        }

        //  add rng number to sign
        if(frameIndex == 1 || frameIndex == 2) {
            SpriteSendable number;
            number.charIndex = charIndex;
            number.mirrored = false;
            number.animationIndex = 33;
            number.frame = sideBStrength - 1;
            number.x = x + xAnimationOffset + xNumberOffset;
            if(mirrored) number.x += x_mirroredOffset + xMirroredNumberOffset - xAnimationOffset;
            number.y = y + yAnimationOffset + yNumberOffset;
            number.continuous = false;
            number.persistent = false;
            number.layer = LAYER_CHARACTER;
            number.framePeriod = 1;

            UART_sendAnimation(number);
        }
    }
    else if(action == GAW_ACTION_DOWNSPECIAL) {
        animationIndex = 34;
        mirrored = l_mirrored;
        disabledFrames = 2;

        if (!btnB && currentTime - holdBucketStartTime > 500) {
            droppingBucket = true;
        }

        if (droppingBucket) {
            xAnimationOffset = 0;
            yAnimationOffset = 0;
            x_mirroredOffset = -14;

            if (mirrored) hitbox.offsetX(0);
            else hitbox.offsetX(0);

            frameIndex = 1;
            frameExtension = 6;
            if(frameLengthCounter++ >= frameExtension) {
                if (y > floor) action = GAW_ACTION_FALLING;
                else if (joyV < -0.3) action = GAW_ACTION_CROUCHING;
                else action = GAW_ACTION_RESTING;
            }
        }
        else {
            if (bucketCount == 0) frameIndex = 0;
            else if (bucketCount == 1) frameIndex = 2;
            else if (bucketCount == 2) frameIndex = 3;
            else if (bucketCount == 3) frameIndex = 4;
            else if (bucketCount >= 4) {
                frameIndex = 5;
                invulnerableFrames = 2;

                int x_mirroredSplashOffset = -90;
                int xSplashOffset = 55;
                int ySplashOffset = 0;

                if (mirrored) xSplashOffset = 0;
                else x_mirroredSplashOffset = 0;

                //  animate splash
                SpriteSendable splash;
                splash.charIndex = charIndex;
                splash.animationIndex = 35;
                splash.frame = 0;
                splash.framePeriod = 1;
                splash.persistent = false;
                splash.continuous = false;
                splash.x = (int16_t) x + x_mirroredSplashOffset + xSplashOffset;
                splash.y = (int16_t) y + ySplashOffset;
                splash.layer = LAYER_NAMETAG;
                splash.mirrored = mirrored;

                UART_sendAnimation(splash);

                hitboxManager->addHurtbox(x + 18, y, mirrored,
                                          downSpecialProjectile, player);

                if(frameLengthCounter == 0) {
                    Audio_destroy(&audio2);
                    Audio_play(GAW_SOUND_DOWNBATTACK, 0.5, &audio2);
                }

                frameExtension = 4;
                if (frameLengthCounter++ >= frameExtension) {
                    droppingBucket = true;
                    bucketCount = 0;
                }

            }

            xAnimationOffset = 0;
            yAnimationOffset = 0;
            x_mirroredOffset = -14;

            if (mirrored) hitbox.offsetX(0);
            else hitbox.offsetX(0);
        }
    }
    else if(action == GAW_ACTION_NEUTRALSPECIAL) {
        animationIndex = 30;
        mirrored = l_mirrored;
        disabledFrames = 2;

        xAnimationOffset = 0;
        yAnimationOffset = 0;
        x_mirroredOffset = -15;

        hitbox.offsetX(-1);

        int frameExtension = 2;
        if(frameLengthCounter++ >= frameExtension) {
            frameLengthCounter = 0;
            frameIndex++;

            //  add a projectile
            if(frameIndex == 1 && projectileCount < 3) {
                //  get a random inactive projectile
                int projectileIndex;
                do { projectileIndex = random(0, 3); }
                while(proj_active[projectileIndex]);

                projectileCount++;
                proj_active[projectileIndex] = true;
                proj_x[projectileIndex] = mirrored ? x : x+27;
                proj_y[projectileIndex] = y + 23;
                proj_xVel[projectileIndex] = mirrored ? -random(15, 25)/10. : random(15, 25)/10.;
                proj_yVel[projectileIndex] = random(70, 90)/10.;
                proj_mirrored[projectileIndex] = mirrored;

                Audio_destroy(&audio2);
                Audio_play(GAW_SOUND_NEUTRALB, 0.5, &audio2);
            }
        }

        if(frameIndex >= 3) {
            if(currentTime - l_btnBRise_t < 300) {
                frameIndex = 0;
                frameLengthCounter = 0;
            }
            else {
                disabledFrames = 4;
                if (y == floor) action = GAW_ACTION_RESTING;
                else action = GAW_ACTION_FALLING;
            }
        }
    }
    else if(action == GAW_ACTION_SHIELD) {
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
            //  drop shield
            Audio_destroy(&audio1);
            Audio_play(SOUND_SHIELDDOWN, 0.5, &audio1);

            shieldDrop_t = currentTime;
            if(y == floor) action = GAW_ACTION_RESTING;
            else action = GAW_ACTION_FALLING;
        }
        else if(shieldDamage > PLAYER_SHIELD_MAXDAMAGE) {
            //  shield break
            Audio_destroy(&audio1);
            Audio_play(SOUND_SHIELDBREAK, 0.5, &audio1);

            action = GAW_ACTION_STUN;
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
    else if(action == GAW_ACTION_HURT) {
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
    if(action == GAW_ACTION_STUN) {
        animationIndex = 43;
        disabledFrames = 2;

        xAnimationOffset = 0;
        yAnimationOffset = 0;
        x_mirroredOffset = 3;

        if(currentTime - stunTimeStart >= PLAYER_STUN_LENGTH_SECONDS * 1000)  {
            if(y == floor) action = GAW_ACTION_RESTING;
            else action = GAW_ACTION_FALLING;
        }

        frameExtension = 7;
        if(frameLengthCounter++ >= frameExtension) {
            frameLengthCounter = 0;
            frameIndex++;
            frameIndex %= 4;
        }
    }
    if(action == GAW_ACTION_PARACHUTE) {
        animationIndex = 37;
        mirrored = l_mirrored;
        disabledFrames = 2;
        noJumpsDisabled = true;

        xAnimationOffset = 0;
        yAnimationOffset = 0;
        x_mirroredOffset = 0;

        x += airSpeed * joyH * 0.8;
        gravityScale = 0.3;
        if(yVel < -3) yVel = -3;

        if(frameIndex == 0) {
            if(mirrored) hitbox.offsetX(8);
            else hitbox.offsetX(16);
        }
        else if(frameIndex == 1) {
            if(mirrored) hitbox.offsetX(8);
            else hitbox.offsetX(20);
        }
        else if(frameIndex == 2) {
            if(mirrored) hitbox.offsetX(8);
            else hitbox.offsetX(16);
        }
        else if(frameIndex == 3) {
            if(mirrored) hitbox.offsetX(14);
            else hitbox.offsetX(10);
        }
        else if(frameIndex == 4) {
            if(mirrored) hitbox.offsetX(20);
            else hitbox.offsetX(4);
        }
        else if(frameIndex == 5) {
            if(mirrored) hitbox.offsetX(22);
            else hitbox.offsetX(0);
        }
        else if(frameIndex == 6) {
            if(mirrored) hitbox.offsetX(20);
            else hitbox.offsetX(4);
        }
        else if(frameIndex == 7) {
            if(mirrored) hitbox.offsetX(14);
            else hitbox.offsetX(16);
        }
        else if(frameIndex == 8) {
            if(mirrored) hitbox.offsetX(8);
            else hitbox.offsetX(20);
        }

        frameExtension = 2;
        if(frameLengthCounter++ >= frameExtension) {
            frameIndex++;
            frameLengthCounter = 0;
        }
        if(frameIndex >= 9) frameIndex = 1;

        if(y <= floor) {
            action = GAW_ACTION_RESTING;
            disabledFrames = 3;
        } else if(joyV < -0.6) {
            action = GAW_ACTION_FALLING;
            disabledFrames = 8;
        }
    }
    if(action == GAW_ACTION_CROUCHING) {

        animationIndex = 6;
        frameIndex = 0;

        xAnimationOffset = 0;
        yAnimationOffset = 0;
        x_mirroredOffset = -3;

        hitbox.offsetY(-8);
        hitbox.offsetHeight(-17);
        hitbox.offsetWidth(6);
        if(mirrored) hitbox.offsetX(2);
        else hitbox.offsetX(4);

        if(onPlatform) {
            action = GAW_ACTION_FALLING;
            y -= 1;
            yVel = 0;
            floor = stage->floor(x + GAW_STAGE_OFFSET, y);
        }

        if(joyV > -0.3) {
            action = GAW_ACTION_RESTING;
            lastBlink = currentTime;

            Audio_destroy(&audio1);
            Audio_play(GAW_SOUND_RISE, 0.5, &audio1);
        }
    }
    if(action == GAW_ACTION_FALLING) {
        if(y <= floor) {
            y = floor;
            yVel = 0;
            l_action = GAW_ACTION_FALLING;
            if(joyH == 0) {
                action = GAW_ACTION_RESTING;
                lastBlink = currentTime;
            } else {
                action = GAW_ACTION_RUNNING;
            }
        }
        else {
            x_mirroredOffset = 0;
            xAnimationOffset = 0;
            yAnimationOffset = 0;
            x += airSpeed * joyH;
            if(x > rightBound) x = rightBound;
            else if(x < leftBound) x = leftBound;

            gravityScale = 1;

            mirrored = l_mirrored;
            hitbox.offsetWidth(4);
            hitbox.offsetY(-1);
            if(mirrored) hitbox.offsetX(4);
            else hitbox.offsetX(2);
            animationIndex = 5;

            frameExtension = 2;
            if (frameLengthCounter++ >= frameExtension) {
                frameLengthCounter = 0;
                frameIndex++;
            }
            if (frameIndex >= 2) frameIndex = 0;
        }
    }
    if(action == GAW_ACTION_RESTING) {
        //  standing, resting

        //  mirrored facing left/right
        mirrored = l_mirrored;

        frameExtension = 8;
        animationIndex = 0;
        x_mirroredOffset = 0;
        xAnimationOffset = 0;
        yAnimationOffset = 0;

        hitbox.offsetHeight(0);
        hitbox.offsetWidth(0);
        hitbox.offsetY(0);
        if(mirrored) hitbox.offsetX(2);
        else hitbox.offsetX(0);

        if (l_action != GAW_ACTION_RESTING || lastBlink == 0) {
            lastBlink = currentTime;
        }

        if (currentTime - lastBlink > blinkPeriod) {
            if (frameLengthCounter++ >= frameExtension) {
                frameLengthCounter = 0;
                frameIndex++;
            }
            if (frameIndex >= 2) {
                frameIndex = 0;
                lastBlink = currentTime;
            }
        } else frameIndex = 0;
    }
    if(action == GAW_ACTION_RUNNING) {
        if(y != floor) {
            action = GAW_ACTION_FALLING;
        }
        else if(joyH == 0) {
            action = GAW_ACTION_RESTING;
        }
        else {
            //  x position
            x += joyH * groundSpeed;
            if(x > rightBound) x = rightBound;
            else if(x < leftBound) x = leftBound;

            //  mirrored facing left/right
            if (joyH == 0) mirrored = l_mirrored;
            else mirrored = joyH < 0;

            //  decide between slow, walk and run animation
            //  dash
            if (absVal(joyH) > 0.6) {
                frameExtension = (uint8_t) ((4 - absVal(0.7 * joyH)));
                if (frameLengthCounter++ > frameExtension) {
                    frameLengthCounter = 0;
                    frameIndex++;
                }
                if (frameIndex >= 2) frameIndex = 0;
                animationIndex = 2;

                if(frameIndex == 0) {
                    hitbox.offsetY(-4);
                    if(mirrored) hitbox.offsetX(1);
                    else hitbox.offsetX(-1);
                    hitbox.offsetHeight(-3);
                    hitbox.offsetWidth(8);
                }
                else if(frameIndex == 1) {
                    hitbox.offsetY(0);
                    if(mirrored) hitbox.offsetX(1);
                    else hitbox.offsetX(-1);
                    hitbox.offsetHeight(4);
                    hitbox.offsetWidth(2);
                }


                xAnimationOffset = -6;
                yAnimationOffset = 0;
                x_mirroredOffset = -3;
                // walk
            } else {
                frameExtension = (uint8_t) ((-2 * absVal(joyH)) + 5);
                if (frameLengthCounter++ > frameExtension) {
                    frameLengthCounter = 0;
                    frameIndex++;
                }
                if (frameIndex >= 3) frameIndex = 1;
                animationIndex = 1;
                if(mirrored) hitbox.offsetX(4);
                else hitbox.offsetX(2);
                hitbox.offsetHeight(4);
                hitbox.offsetWidth(3);

                xAnimationOffset = 0;
                yAnimationOffset = 0;
                x_mirroredOffset = 0;
            }

            //  play audio whenever it's done
            if(audio1 == -1) Audio_play(GAW_SOUND_STEP, 0.5, &audio1);
        }
    }

    l_action = action;

    //  regenerate shield
    if(action != GAW_ACTION_SHIELD) {
        shieldDamage -= PLAYER_SHIELD_REGEN;
        if(shieldDamage < 0) shieldDamage = 0;
    }

    //  disabled means can interrupt current action and start new action
    if(disabledFrames > 0) disabledFrames--;
    if(invulnerableFrames > 0) invulnerableFrames--;
    if(disabledFrames == -1) {
        //  knockback stun, remove stun when falling or on floor
        if(y == floor || yVel < 0) {
            if(y > floor) action = GAW_ACTION_FALLING;
            disabledFrames = 0;
        }
    }

    if(x > rightBound) x = rightBound;
    else if(x < leftBound) x = leftBound;

    this->hitbox.initialize(x+15, y+15, 28, 30);

    if(!mirrored) x_mirroredOffset = 0;
    else x_mirroredOffset -= xAnimationOffset;

    //  handle all the neutral b projectiles
    for(int i = 0; i < 4 && projectileCount > 0; i++) {
        if(!proj_active[i]) continue;
        SpriteSendable projectile;
        projectile.charIndex = charIndex;
        projectile.animationIndex = 31;
        projectile.framePeriod = 1;
        projectile.frame = i;
        projectile.persistent = false;
        projectile.continuous = false;
        projectile.x = proj_x[i];
        projectile.y = proj_y[i];
        projectile.mirrored = proj_mirrored[i];
        projectile.layer = LAYER_CHARACTER_PROJECTILE;

        UART_sendAnimation(projectile);

        //  add hurtbox
        hitboxManager->addHurtbox(proj_x[i]+7, proj_y[i], mirrored,
                                  neutralSpecialProjectile, player);

        bool killProjectile = false;

        //  will the projectile fall on the floor?
        if(proj_y[i] + proj_yVel[i]< stage->floor(proj_x[i] + proj_xVel[i], proj_y[i])) killProjectile = true;

        //  is projectile out of the screen?
        if(proj_y[i] < 0) killProjectile = true;

        proj_x[i] += proj_xVel[i];
        proj_y[i] += proj_yVel[i];

        proj_yVel[i] -= gravityFalling;

        if(killProjectile) {
            proj_active[i] = false;
            projectileCount--;
        }
    }

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
    if(!overrideMaxVelocity && yVel < maxFallingVelocity) yVel = maxFallingVelocity;
    overrideMaxVelocity = false;

    y += yVel;
    if(y > ceiling && action != GAW_ACTION_LEDGEGRAB) {
        y = ceiling;
        yVel = 0;
    }

    if(y <= floor) {

        //  just landed!
        if(!l_onFloor) {
            Audio_destroy(&audio1);
            Audio_play(GAW_SOUND_LANDING, 0.5, &audio1);
        }

        l_onFloor = true;
        y = floor;
        yVel = 0;
        jumpsUsed = 0;
    }
    else l_onFloor = false;

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
       (action == GAW_ACTION_RESTING) && currentTime - l_btnARise_t == 0) {
        action = GAW_ACTION_NEUTRALATTACK;
        disabledFrames = 2;
        frameIndex = 0;
        frameLengthCounter = 0;

    }
        //  forward smash
    else if(disabledFrames == 0 && y == floor && (action == GAW_ACTION_RUNNING || action == GAW_ACTION_RESTING)
            && currentTime-l_btnARise_t == 0 && absVal(joyH - l_joyH) > 0.5) {
        action = GAW_ACTION_FORWARDSMASHHOLD;
        mirrored = joyH < 0;
        disabledFrames = 2;
        frameIndex = 0;
        frameLengthCounter = 0;
        fsmash_startTime = currentTime;

        Audio_destroy(&audio2);
        Audio_play(GAW_SOUND_SMASHHOLDSTART, 0.5, &audio2);
    }
        //  down smash
    else if(disabledFrames == 0 && y == floor && (action == GAW_ACTION_RUNNING || action == GAW_ACTION_RESTING)
            && currentTime-l_btnARise_t == 0 && joyV - l_joyV < -0.5) {
        action = GAW_ACTION_DOWNSMASHHOLD;
        mirrored = l_mirrored;
        disabledFrames = 2;
        frameIndex = 0;
        frameLengthCounter = 0;
        dsmash_startTime = currentTime;

        Audio_destroy(&audio2);
        Audio_play(GAW_SOUND_SMASHHOLDSTART, 0.5, &audio2);
    }
        //  up smash
    else if(disabledFrames == 0 && y == floor && (action == GAW_ACTION_RUNNING || action == GAW_ACTION_RESTING)
            && currentTime-l_btnARise_t == 0 && joyV - l_joyV > 0.5) {
        action = GAW_ACTION_UPSMASHHOLD;
        mirrored = l_mirrored;
        disabledFrames = 2;
        frameIndex = 0;
        frameLengthCounter = 0;
        usmash_startTime = currentTime;

        Audio_destroy(&audio2);
        Audio_play(GAW_SOUND_SMASHHOLDSTART, 0.5, &audio2);
    }
        //  down tilt
    else if(disabledFrames == 0 && y == floor && (GAW_ACTION_CROUCHING || action == GAW_ACTION_RESTING)
            && currentTime - l_btnARise_t == 0 && joyV < -0.3) {
        action = GAW_ACTION_DOWNTILT;
        disabledFrames = 2;
        frameIndex = 0;
        frameLengthCounter = 0;

        Audio_destroy(&audio2);
        Audio_play(GAW_SOUND_DOWNTILT, 0.5, &audio2);
    }
        //  neutral air
    else if(disabledFrames == 0 && y > floor
            && (action == GAW_ACTION_FALLING || action == GAW_ACTION_JUMPING
                || action == GAW_ACTION_DOUBLEJUMPING)
            && currentTime - l_btnARise_t == 0
            && absVal(joyH) < 0.3 && absVal(joyV) < 0.3) {
        action = GAW_ACTION_NEUTRALAIR;
        mirrored = l_mirrored;
        disabledFrames = 2;
        frameIndex = 0;
        frameLengthCounter = 0;

        Audio_destroy(&audio2);
        Audio_play(GAW_SOUND_NEUTRALAIR, 0.5, &audio2);
    }
        //  down air
    else if(disabledFrames == 0 && y > floor
            && (action == GAW_ACTION_FALLING || action == GAW_ACTION_JUMPING
                || action == GAW_ACTION_DOUBLEJUMPING)
            && currentTime - l_btnARise_t == 0 && joyV < -0.3) {
        action = GAW_ACTION_DOWNAIR;
        if(joyH == 0) mirrored = l_mirrored;
        else mirrored = joyH < 0;
        disabledFrames = 2;
        frameIndex = 0;
        frameLengthCounter = 0;

        Audio_destroy(&audio2);
        Audio_play(GAW_SOUND_DOWNAIR, 0.5, &audio2);
    }
        //  up air
    else if(disabledFrames == 0 && y > floor
            && (action == GAW_ACTION_FALLING || action == GAW_ACTION_JUMPING
                || action == GAW_ACTION_DOUBLEJUMPING)
            && currentTime - l_btnARise_t == 0 && joyV > 0.3) {
        action = GAW_ACTION_UPAIR;
        if(joyH == 0) mirrored = l_mirrored;
        else mirrored = joyH < 0;
        disabledFrames = 2;
        frameIndex = 0;
        frameLengthCounter = 0;
    }
        //  forward air
    else if(disabledFrames == 0 && y > floor
            && (action == GAW_ACTION_FALLING || action == GAW_ACTION_JUMPING
                || action == GAW_ACTION_DOUBLEJUMPING)
            && currentTime - l_btnARise_t == 0
            && ( (joyH > 0.3 && !mirrored) || (joyH < -0.3 && mirrored) ) ) {
        action = GAW_ACTION_FORWARDAIR;
        disabledFrames = 2;
        frameIndex = 0;
        frameLengthCounter = 0;

        Audio_destroy(&audio2);
        Audio_play(GAW_SOUND_FORWARDAIR, 0.5, &audio2);
    }
        //  back air
    else if(disabledFrames == 0 && y > floor
            && (action == GAW_ACTION_FALLING || action == GAW_ACTION_JUMPING
                || action == GAW_ACTION_DOUBLEJUMPING)
            && currentTime - l_btnARise_t == 0
            && ( (joyH < -0.3 && !mirrored) || (joyH > 0.3 && mirrored) ) ) {
        action = GAW_ACTION_BACKAIR;
        disabledFrames = 2;
        frameIndex = 0;
        frameLengthCounter = 0;

        Audio_destroy(&audio2);
        Audio_play(GAW_SOUND_BACKAIR, 0.5, &audio2);
    }
        //  forward tilt
    else if(disabledFrames == 0 && y == floor && (action == GAW_ACTION_RUNNING || action == GAW_ACTION_RESTING)
            && currentTime - l_btnARise_t == 0 && absVal(joyH) < 0.6 && absVal(joyH) > 0.1) {
        action = GAW_ACTION_FORWARDTILT;
        mirrored = joyH < 0;
        disabledFrames = 2;
        frameIndex = 0;
        frameLengthCounter = 0;

        Audio_destroy(&audio2);
        Audio_play(GAW_SOUND_FORWARDTILT, 0.5, &audio2);
    }
        //  up tilt
    else if(disabledFrames == 0 && y == floor && (action == GAW_ACTION_RUNNING || action == GAW_ACTION_RESTING)
            && currentTime - l_btnARise_t == 0 && joyV > 0) {
        action = GAW_ACTION_UPTILT;
        disabledFrames = 2;
        frameIndex = 0;
        frameLengthCounter = 0;

        Audio_destroy(&audio2);
        Audio_play(GAW_SOUND_UPTILT, 0.5, &audio2);
    }
        //  dash attack
    else if(disabledFrames == 0 && y == floor && action == GAW_ACTION_RUNNING
            && currentTime - l_btnARise_t == 0 && absVal(joyH) > 0.4) {
        action = GAW_ACTION_DASHATTACK;
        mirrored = joyH < 0;
        disabledFrames = 2;
        frameIndex = 0;
        frameLengthCounter = 0;
        dashAttackStartTime = currentTime;
    }
        //  down B bucket
    else if(disabledFrames == 0 && currentTime - l_btnBRise_t == 0
            && joyV < -0.5) {
        action = GAW_ACTION_DOWNSPECIAL;
        disabledFrames = 2;
        frameIndex = 0;
        frameLengthCounter = 0;
        mirrored = l_mirrored;

        holdBucketStartTime = currentTime;
        droppingBucket = false;

        Audio_destroy(&audio1);
        Audio_play(GAW_SOUND_DOWNB, 0.5, &audio1);
    }
        //  up special
    else if(!noJumpsDisabled &&
            ( (action == GAW_ACTION_FALLING || action == GAW_ACTION_JUMPING  ||
               action == GAW_ACTION_DOUBLEJUMPING) ||
              (y == floor && (action == GAW_ACTION_RESTING || action == GAW_ACTION_RUNNING)) ) &&
            currentTime-l_btnBRise_t == 0 && joyV > 0.3) {
        action = GAW_ACTION_UPSPECIAL;
        if(joyH == 0) mirrored = l_mirrored;
        else mirrored = joyH < 0;
        disabledFrames = 2;
        frameIndex = 0;
        frameLengthCounter = 0;

        yVel = 8.5;
        y++;

        Audio_destroy(&audio2);
        Audio_play(GAW_SOUND_UPB, 0.5, &audio2);
    }
        //  side special
    else if(disabledFrames == 0 && currentTime - l_btnBRise_t == 0
            && absVal(joyH) >= 0.5) {
        action = GAW_ACTION_SIDESPECIAL;
        disabledFrames = 2;
        frameIndex = 0;
        frameLengthCounter = 0;
        mirrored = joyH < 0;
        sideBStrength = random(1, 9);

        Audio_destroy(&audio2);
        Audio_play(GAW_SOUND_SIDEB, 0.5, &audio2);
    }
        //  neutral B
    else if(disabledFrames == 0 && currentTime - l_btnBRise_t == 0
            && absVal(joyH) < 0.5) {
        action = GAW_ACTION_NEUTRALSPECIAL;
        disabledFrames = 2;
        frameIndex = 0;
        frameLengthCounter = 0;
        mirrored = l_mirrored;
    }

        //  movement
        //  jumping
    else if((disabledFrames == 0 &&
             (action == GAW_ACTION_RESTING || action == GAW_ACTION_CROUCHING || action == GAW_ACTION_RUNNING
              || action == GAW_ACTION_HURT)
             && (joyV - l_joyV) > joystickJumpSpeed && l_joyV > -0.1 && y == floor)
            || (action == GAW_ACTION_LEDGEGRAB && (joyV - l_joyV) > joystickJumpSpeed && l_joyV > -0.1
                && disabledFrames == 0)) {
        jumpsUsed = 0;
        disabledFrames = 1;
        yVel = initialJumpSpeed;
        l_action = action;
        action = GAW_ACTION_JUMPING;
        frameIndex = 0;
        frameLengthCounter = 0;
        ledgeGrabTime = currentTime;

        Audio_destroy(&audio1);
        Audio_play(GAW_SOUND_JUMP, 0.5, &audio1);
    }
        //  double jump
    else if( disabledFrames == 0
             && ( (action == GAW_ACTION_JUMPING || action == GAW_ACTION_FALLING
                   || action == GAW_ACTION_DOUBLEJUMPING
                   || (action == GAW_ACTION_FORWARDAIR || action == GAW_ACTION_BACKAIR|| action == GAW_ACTION_UPAIR) )
                  && jumpsUsed == 0 && (joyV - l_joyV) > joystickJumpSpeed && l_joyV > -0.1) ) {
        jumpsUsed++;
        yVel = repeatedJumpSpeed;
        l_action = action;
        action = GAW_ACTION_DOUBLEJUMPING;
        frameIndex = 0;
        frameLengthCounter = 0;

        if(joyH == 0) mirrored = l_mirrored;
        else mirrored = joyH < 0;

        Audio_destroy(&audio1);
        Audio_play(GAW_SOUND_DOUBLEJUMP, 0.5, &audio1);
    }
        //  shield
    else if(disabledFrames == 0 &&
            ( (action == GAW_ACTION_FALLING || action == GAW_ACTION_JUMPING  ||
               action == GAW_ACTION_DOUBLEJUMPING) ||
              (y == floor && (action == GAW_ACTION_RESTING || action == GAW_ACTION_RUNNING ||
                              action == GAW_ACTION_CROUCHING)) )
            && shield && !l_shield && (PLAYER_SHIELD_MAXDAMAGE - shieldDamage > 10)
            && currentTime - shieldDrop_t > 300) {
        action = GAW_ACTION_SHIELD;
        disabledFrames = 2;

        Audio_destroy(&audio1);
        Audio_play(SOUND_SHIELDUP, 0.5, &audio1);
    }
        //  running/walking
    else if(((action == GAW_ACTION_RESTING) || (disabledFrames == 0 && action == GAW_ACTION_HURT))
            && absVal(joyH) > 0) {
        l_action = action;
        action = GAW_ACTION_RUNNING;
        frameIndex = 0;
        frameLengthCounter = 0;
    }
        //  crouching
    else if((action == GAW_ACTION_RESTING || action == GAW_ACTION_RUNNING || action == GAW_ACTION_HURT) &&
            joyV < -0.5 && y == floor) {
        l_action = action;
        action = GAW_ACTION_CROUCHING;

        frameIndex = 0;
        frameLengthCounter = 0;

        Audio_destroy(&audio1);
        Audio_play(GAW_SOUND_CROUCH, 0.5, &audio1);
    }
        //  resting
    else if(disabledFrames == 0 &&
            joyH == 0 && joyV == 0 && (action == GAW_ACTION_FALLING || action == GAW_ACTION_HURT)) {
        l_action = action;
        if(y == floor) action = GAW_ACTION_RESTING;
        else action = GAW_ACTION_FALLING;

        frameIndex = 0;
        frameLengthCounter = 0;
    }
        //  ledge grab drop down
    else if(disabledFrames == 0 && (action == GAW_ACTION_LEDGEGRAB) &&
            joyV < -0.3) {
        action = GAW_ACTION_FALLING;
        y -= 30;
        ledgeGrabTime = currentTime;
    }

    updateLastValues(joyH, joyV, btnA, btnB, shield);
}

void GameandWatch::updateLastValues(float joyH, float joyV, bool btnA, bool btnB, bool shield) {
    l_joyH = joyH;
    l_joyV = joyV;
    l_btnA = btnA;
    l_btnB = btnB;
    l_shield = shield;

    l_mirrored = mirrored;
}

void GameandWatch::collide(Hurtbox *hurtbox, Player *otherPlayer) {
    //  ledge grab
    if(hurtbox->source == 0) {
        if(this->hitbox.y < hurtbox->y
           && currentTime - ledgeGrabTime > 1000
           && yVel <= 0 && action != GAW_ACTION_SHIELD) {
            action = GAW_ACTION_LEDGEGRAB;
            mirrored = hurtbox->damage != 0;
            yVel = 0;
            xVel = 0;
            x = hurtbox->x;
            y = hurtbox->y;
            disabledFrames = 6;
            jumpsUsed = 0;

            Audio_destroy(&audio1);
            Audio_play(SOUND_LEDGECATCH, 0.5, &audio1);
        }
        return;
    }
    else if(action == GAW_ACTION_DOWNSPECIAL && hurtbox->isProjectile && currentTime - lastBucket > 750) {
        bucketCount++;
        lastBucket = currentTime;
        droppingBucket = true;

        invulnerableFrames = 3;

        if(hurtbox->activationFlagPointer != nullptr) {
            *(hurtbox->activationFlagPointer) = true;
        }

        Audio_destroy(&audio1);
        Audio_play(GAW_SOUND_DOWNBABSORB, 0.5, &audio1);
    }
    else if(action == GAW_ACTION_SHIELD) {
        if(hurtbox->damage < PLAYER_SHIELD_MAXDAMAGE/2.) shieldDamage += hurtbox->damage * 0.3;
        else shieldDamage += PLAYER_SHIELD_MAXDAMAGE/2.;

        if(hurtbox->activationFlagPointer != nullptr) {
            *(hurtbox->activationFlagPointer) = true;
        }
    }
        // only knockback if not currently knocked back
    else if(disabledFrames != -1 && invulnerableFrames == 0
    && !(hurtbox->isProjectile && currentTime - lastBucket <= 750)) {
        disabledFrames = hurtbox->stunFrames;
        damage += hurtbox->damage;

        float knockbackMultiplier = damage / 130. + 1.0;

        if (otherPlayer->x < x) xVel = hurtbox->xKnockback * knockbackMultiplier;
        else xVel = -hurtbox->xKnockback * knockbackMultiplier;
        yVel = hurtbox->yKnockback * knockbackMultiplier;

        if(hurtbox->activationFlagPointer != nullptr) {
            *(hurtbox->activationFlagPointer) = true;
        }

        action = GAW_ACTION_HURT;

        Audio_destroy(&audio1);
        Audio_destroy(&audio2);
        if(hurtbox->xKnockback * knockbackMultiplier >= 3.0) {
            int rand = random(1, 4);
            if(rand == 1) Audio_play(SOUND_HIT1, 0.5, &audio2);
            else if(rand == 2) Audio_play(SOUND_HIT2, 0.5, &audio2);
            else if(rand == 3) Audio_play(SOUND_HIT3, 0.5, &audio2);
            else if(rand == 4) Audio_play(SOUND_HIT4, 0.5, &audio2);
        }
        if(hurtbox->xKnockback * knockbackMultiplier >= 5.0) {
            Audio_play(SOUND_CROWDCHEER, 0.5, &audio1);
        }
    }

}

void GameandWatch::reset() {
    action = GAW_ACTION_RESTING;
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

    proj_active[0] = false;
    proj_active[1] = false;
    proj_active[2] = false;
    proj_active[3] = false;
    projectileCount = 0;

    shieldDamage = 0;
    bucketCount = 0;
    lastBucket = 0;

    dead = false;
    l_onFloor = true;
}


