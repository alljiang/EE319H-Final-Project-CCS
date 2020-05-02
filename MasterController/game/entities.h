//
// Created by Allen on 2/28/2020.
//

#include "stdint.h"

#ifndef EE319K_FINAL_PROJECT_INITIAL_TESTING_ENTITIES_H
#define EE319K_FINAL_PROJECT_INITIAL_TESTING_ENTITIES_H


class Player;

class Collider {

#define SHAPE_CIRCLE 0
#define SHAPE_RECTANGLE 1

public:
    short x, y;
    short xOffset, yOffset;
    uint8_t shape;
    short height=0, width=0, radius=0;

    Collider(short x, short y, uint8_t shape, short radius) {
        this->x = x;
        this->y = y;
        this->shape = shape;
        this->radius = radius;
    }

    Collider(short x, short y, uint8_t shape, short length, short width) {
        this->x = x;
        this->y = y;
        this->shape = shape;
        this->height = length;
        this->width = width;
    }

};

class Hurtbox: public Collider {

public:
    float damage;
    int8_t source;     //  player who created this hitbox, will not damage this player
    int8_t frames, currentFrame;
    bool active{false}, isProjectile{false};
    float xKnockback, yKnockback;
    int16_t stunFrames;
    bool* activationFlagPointer;

    Hurtbox() : Collider(0,0,0,0) {}

    Hurtbox(bool circle, short cX, short cY,
            uint8_t boxShape, short radius,
            int8_t frames=1, float damage=0,
            float xknockback=0, float yknockback=0,
            int16_t stunFrames=0, bool isProjectile=false)
            : Collider(cX, cY, boxShape, radius) {
        shape = boxShape;
        this->frames = frames;
        this->damage = damage;
        this->xKnockback = xknockback;
        this->yKnockback = yknockback;
        this->stunFrames = stunFrames;
        this->isProjectile = isProjectile;
    }

    Hurtbox(short cX, short cY, uint8_t boxShape, short height, short width,
            int8_t frames=1, float damage=0,
            float xknockback=0, float yknockback=0,
            int16_t stunFrames=0, bool isProjectile=false, bool* activationFlag=nullptr)
            : Collider(cX, cY, boxShape, height, width) {
        shape = boxShape;
        this->frames = frames;
        this->damage = damage;
        this->xKnockback = xknockback;
        this->yKnockback = yknockback;
        this->stunFrames = stunFrames;
        this->isProjectile = isProjectile;
        this->activationFlagPointer = activationFlag;
    }

    //  if source is 0, hurtbox is a grabbable stage ledge
    void setSource(uint8_t playerSource) { this->source = playerSource; }
};

class Hitbox: public Collider {

public:
    float radiusOffset{0};
    float heightOffset{0};
    float widthOffset{0};

    Hitbox(float cX, float cY, float radius)
            : Collider(cX, cY, SHAPE_CIRCLE, radius) {}
    Hitbox(float cX, float cY, float length, float width)
            : Collider(cX, cY, SHAPE_RECTANGLE, length, width) {}

    void offsetY(float yOffset) { this->yOffset = yOffset; }
    void offsetX(float xOffset) { this->xOffset = xOffset; }
    void offsetX(float xOffset, bool mirrored) { if(mirrored) offsetX(-xOffset); else offsetX(xOffset); }

    void offsetRadius(float radius) { this->radiusOffset = radius; };
    void offsetHeight(float height) { this->heightOffset = height; }
    void offsetWidth(float width) { this->widthOffset = width; }

    void initialize(float cX, float cY, float radius) {
        x = cX;
        y = cY;
        shape = SHAPE_CIRCLE;
        this->radius = radius;
    }

    void initialize(float cX, float cY, float width, float height) {
        x = cX;
        y = cY;
        shape = SHAPE_RECTANGLE;
        this->width = width;
        this->height = height;
    }

    bool isColliding(class Hurtbox hurtbox);
};

class HitboxManager {

#define hurtboxSlots 16

protected:
    Player *p1, *p2;
    Hurtbox hurtboxes[hurtboxSlots];
    bool activationFlags[hurtboxSlots];
    uint16_t persistentHurtbox = 0; // big endian

public:

    void initialize(class Player* player1, class Player* player2 = nullptr) {
        p1 = player1;
        if(player2 != nullptr) p2 = player2;
        else p2 = nullptr;

        //  clear hitboxes
        for(int i = 0; i < hurtboxSlots; i++) {
            hurtboxes[i].active = false;
            activationFlags[i] = false;
        }
        persistentHurtbox = 0;
    }

    void checkCollisions();
    bool* addHurtboxFullConfig(float xOffset, float yOffset, bool mirrored,
                              class Hurtbox hurtBox, uint8_t playerSource, bool persistent);
    bool* addHurtbox(float xOffset, float yOffset, bool mirrored,
                    class Hurtbox hurtBox, uint8_t playerSource, float multiplier=1);
    void displayHitboxesOverlay();
    void clearHitboxOverlay();
};

class Entity {

public:
    float x; //  [0, 320]
    float y;  //  [0, 240]
    float yVel;    //  pps
    float xVel;

    long long currentTime = 0;

    void setX(float newX) { x = newX; }
    void setY(float newY) { y = newY; }

};

class Player: public Entity {

#define PLAYER_SHIELD_MAXDAMAGE 30
#define PLAYER_SHIELD_REGEN 0.375
#define PLAYER_SHIELD_DEGEN 0.375
#define PLAYER_STUN_LENGTH_SECONDS 7

protected:
    const float joystickJumpSpeed = 0.4;   //  joystick must change by this much to activate a jump

    uint8_t player;             //  1 or 2
    float damage;            //  percentage between 0% and 999%
    int16_t action, l_action;

    uint8_t animationIndex;     //  index of animation
    uint8_t frameIndex{0};      //  current frame index of current animation
    uint8_t frameExtension{1}, frameLengthCounter{0};  //  counts current frame height
    bool continuous{false};
    bool mirrored, l_mirrored;

    long long deathTime{0};

    long long disabledFrames {0};   //  frames before making a new move
    long long invulnerableFrames {0};   //  frames of hurtbox invulnerability
    bool noJumpsDisabled;           //  disabled until landing because of running out of jumps

    float l_joyH;              //  last joystick horizontal value
    float l_joyV;              //  last joystick vertical value
    bool l_btnA;                //  last button A value
    long long l_btnARise_t;     //  last button A pressed time in millis
    long long l_btnAFall_t;     //  last button A release time in millis
    bool l_btnB;                //  last button B value
    long long l_btnBRise_t;     //  last button B pressed time in millis
    long long l_btnBFall_t;     //  last button B release time in millis
    bool l_shield;              //  last shield value
    long long l_shieldRise_t;   //  last shield pressed time in millis
    long long l_shieldFall_t;   //  last shield release time in millis

    //  smash attacks
    long long fsmash_startTime, dsmash_startTime, usmash_startTime;

    //  ledge grab
    long long ledgeGrabTime{0};

    //  jumping
    uint8_t jumpsUsed;  // midair only

    //  shielding
    float shieldDamage;

    //  stunned
    long long stunTimeStart;

public:
    Player() {}

    bool dead;
    uint8_t stocksRemaining;

    Hitbox hitbox = Hitbox(0, 0, 0);

    void setPlayer(uint8_t p) { player = p; }
    void setMirrored(bool mirror) { l_mirrored = mirrored = mirror; }
    void setStocks(uint8_t lives) { stocksRemaining = lives; }

    virtual void controlLoop(float joyH, float joyV, bool btnA, bool btnB, bool shield, class Stage* stage,
                             class HitboxManager* hitboxManager) = 0; //  called every update
    virtual void updateLastValues(float joyH, float joyV, bool btnA, bool btnB, bool shield) = 0;
    virtual void collide(class Hurtbox *hurtbox, class Player *otherPlayer) = 0;
    virtual void reset() = 0;
};


class Kirby: public Player {

#define KIRBY_ACTION_RESTING 0
#define KIRBY_ACTION_RUNNING 1
#define KIRBY_ACTION_FALLING 2
#define KIRBY_ACTION_JUMPING 3
#define KIRBY_ACTION_MULTIJUMPING 4
#define KIRBY_ACTION_CROUCHING 5
#define KIRBY_ACTION_JABSINGLE 6
#define KIRBY_ACTION_JABDOUBLE 7
#define KIRBY_ACTION_JABREPEATING 8
#define KIRBY_ACTION_FORWARDTILT 9
#define KIRBY_ACTION_DOWNTILT 10
#define KIRBY_ACTION_FORWARDSMASHHOLD 11
#define KIRBY_ACTION_FORWARDSMASH 12
#define KIRBY_ACTION_DOWNSMASHHOLD 13
#define KIRBY_ACTION_DOWNSMASH 14
#define KIRBY_ACTION_UPSMASHHOLD 15
#define KIRBY_ACTION_UPSMASH 16
#define KIRBY_ACTION_DASHATTACK 18
#define KIRBY_ACTION_UPTILT 20
#define KIRBY_ACTION_UPSPECIALINITIAL 25
#define KIRBY_ACTION_UPSPECIALRISING 26
#define KIRBY_ACTION_UPSPECIALTOP 27
#define KIRBY_ACTION_UPSPECIALFALLING 28
#define KIRBY_ACTION_LEDGEGRAB 30
#define KIRBY_ACTION_BACKAIR 40
#define KIRBY_ACTION_DOWNAIR 41
#define KIRBY_ACTION_FORWARDAIR 42
#define KIRBY_ACTION_UPAIR 43
#define KIRBY_ACTION_NEUTRALAIR 44
#define KIRBY_ACTION_NEUTRALSPECIAL 50
#define KIRBY_ACTION_SIDESPECIALCHARGE 53
#define KIRBY_ACTION_SIDESPECIALRELEASE 54
#define KIRBY_ACTION_DOWNSPECIALMORPH 56
#define KIRBY_ACTION_DOWNSPECIALFALL 57
#define KIRBY_ACTION_DOWNSPECIALUNMORPH 58
#define KIRBY_ACTION_HURT 60
#define KIRBY_ACTION_SHIELD 61
#define KIRBY_ACTION_STUN 62

#define KIRBY_STAGE_OFFSET 18

protected:
    //  animation config
    const uint8_t charIndex = 0;
    const uint16_t blinkPeriod = 2000;      //  how often kirby blinks when standing still

    //  physics config
    const float groundSpeed = 1.8*3;  // pps
    const float airSpeed = 1.1*3;

    const float initialJumpSpeed = 1.5*3;
    const float repeatedJumpSpeed = 1.3*3;
    const float gravityRising = 0.07*7;
    const float gravityFalling = 0.1*7;
    const float maxFallingVelocity = -2.3*3;

    const float airResistance = .2;
    const float maxHorizontalSpeed = 20*3;
    const float groundFriction = 0.5;

    const float DIVerticalSpeed = 0.2 * 3;
    const float DIHorizontalSpeed = 0.2 * 3;
    const float DIKnockbackVerticalSpeed = 0.3 * 3;
    const float DIKnockbackHorizontalSpeed = 0.4 * 3;

    //  standing, resting
    long long lastBlink{0};

    //  jabbing
    long long l_singleJabTime, l_doubleJabTime;

    //  up special
    float startY;
    float upb_projectile_x, upb_projectile_startX, upb_projectile_startY;
    bool upb_projectile_mirrored;
    bool upb_projectile_active;
    bool* upb_projectile_activationFlag = nullptr;

    //  down special
    long long morphEndTime, morphLandTime;

    //  side special
    long long hammerChargeStartTime;
    uint32_t hammerChargeTime;

    //  neutral special projectile
    bool starProjActive;
    long long starProjStartTime;
    uint8_t starProjFrameIndex;
    uint8_t starProjFrameCounter;
    bool starProjMirrored;
    float starProj_x, starProj_y;

public:
    Hurtbox jabSingle = Hurtbox(true,14, 11, SHAPE_CIRCLE,
                                8, 1,
                                0.8, 0, 0,3);
    Hurtbox jabDouble = Hurtbox(true,14, 12, SHAPE_CIRCLE,
                                7, 1,
                                0.8, 0, 0,3);
    Hurtbox jabRepeating0 = Hurtbox(true,23, 12, SHAPE_CIRCLE,
                                    11, 1,
                                    0.2, 0, 0,3);
    Hurtbox jabRepeating1 = Hurtbox(true,25, 25, SHAPE_CIRCLE,
                                    11, 1,
                                    0.2, 0, 0,3);
    Hurtbox jabRepeating2 = Hurtbox(true,23, 5, SHAPE_CIRCLE,
                                    9, 1,
                                    0.2, 0, 0,3);
    Hurtbox forwardTilt = Hurtbox(true,10, 11, SHAPE_CIRCLE,
                                  8, 1,
                                  3, 3.0, 2.5, -1);
    Hurtbox upTilt = Hurtbox((short)-6, 21, SHAPE_RECTANGLE,
                             25, 18, 1,
                             3, 1.5, 3.3, -1);
    Hurtbox downTilt = Hurtbox((short)4, 5, SHAPE_RECTANGLE,
                               10, 33, 1,
                               3, 1, 2.6, -1);
    Hurtbox forwardSmash = Hurtbox((short)25, 5, SHAPE_RECTANGLE,
                                   20, 20, 1,
                                   3.9, 3.7, 3.1, -1);
    Hurtbox upSmash = Hurtbox(true, 0., 26, SHAPE_CIRCLE,
                              14, 1,
                              3.9,3.1, 4.1,-1);
    Hurtbox downSmash = Hurtbox((short)0, 5, SHAPE_RECTANGLE,
                                10, 35, 1,
                                3.9,4.2, 1.5, -1);
    Hurtbox upSpecialRising = Hurtbox((short)25, 18, SHAPE_RECTANGLE,
                                      20, 30, 1,
                                      3, 2.1, 3.2, -1);
    Hurtbox upSpecialTop = Hurtbox((short)0, 40, SHAPE_RECTANGLE,
                                   20, 40, 1,
                                   2.2, 2.8, 3.3, -1);
    Hurtbox upSpecialFalling = Hurtbox((short)25, 18, SHAPE_RECTANGLE,
                                      20, 30, 1,
                                      1.2, 2.8, -0.8, -1);
    Hurtbox upSpecialProjectile = Hurtbox((short)4, 16, SHAPE_RECTANGLE,
                                          32, 20, 1,
                                          4, 3.2, 2.9, 7, true);
    Hurtbox downSpecial = Hurtbox(true,0, 5, SHAPE_CIRCLE,
                                  12, 1,
                                  6, 3.8, 4.9, -1);
    Hurtbox neutralAir = Hurtbox(true,0, 15, SHAPE_CIRCLE,
                                 14, 1,
                                 4, 1.9, 3.0, -1);
    Hurtbox forwardAir = Hurtbox(true,18, 13, SHAPE_CIRCLE,
                                 8, 1,
                                 4.5, 3.3, 2.0, -1);
    Hurtbox backAir = Hurtbox(true,-14, 11, SHAPE_CIRCLE,
                              6.0, 1,
                              6, 3.7, 2.3, -1);
    Hurtbox upAir = Hurtbox(true,0, 28, SHAPE_CIRCLE,
                            16, 1,
                            6, 1.6, 2.7, -1);
    Hurtbox downAir = Hurtbox((short)5, -2, SHAPE_RECTANGLE,
                              17, 10, 1,
                              0.8, 0.6, 0.1, 2);
    Hurtbox dashAttack = Hurtbox(true,2, 13, SHAPE_CIRCLE,
                                 14, 1,
                                 4, 4, 4.7, -1);
    Hurtbox sideSpecial0 = Hurtbox(true,-28, 7, SHAPE_CIRCLE,
                                   11, 1);
    Hurtbox sideSpecial1 = Hurtbox(true,-17, 3, SHAPE_CIRCLE,
                                   11, 1,
                                   5, 3.5, 3.7, -1);
    Hurtbox sideSpecial2 = Hurtbox(true,0, 2, SHAPE_CIRCLE,
                                   11, 1,
                                   5, 3.5, 3.7, -1);
    Hurtbox sideSpecial3 = Hurtbox(true,15, 5, SHAPE_CIRCLE,
                                   11, 1,
                                   5, 3.5, 3.7, -1);
    Hurtbox sideSpecial4 = Hurtbox(true,30, 6, SHAPE_CIRCLE,
                                   11, 1,
                                   5, 3.5, 3.7, -1);
    Hurtbox sideSpecial5 = Hurtbox(true,17, 10, SHAPE_CIRCLE,
                                   11, 1,
                                   5, 3.5, 3.7, -1);
    Hurtbox starProjectile = Hurtbox(true, 0, 13, SHAPE_CIRCLE,
                                     13, 1,
                                    7, 3.0, 3.2, -1, true);

    //  general control loop
    void controlLoop(float joyH, float joyV, bool btnA, bool btnB, bool shield, class Stage* stage,
                     class HitboxManager* hitboxManager) override; //  called every update

    void updateLastValues(float joyH, float joyV, bool btnA, bool btnB, bool shield) override;

    void collide(class Hurtbox *hurtbox, class Player *otherPlayer) override;

    void reset() override;
};

class GameandWatch: public Player {

#define GAW_ACTION_RESTING 0
#define GAW_ACTION_RUNNING 1
#define GAW_ACTION_FALLING 2
#define GAW_ACTION_JUMPING 3
#define GAW_ACTION_DOUBLEJUMPING 4
#define GAW_ACTION_CROUCHING 5
#define GAW_ACTION_NEUTRALATTACK 6
#define GAW_ACTION_FORWARDTILT 7
#define GAW_ACTION_DOWNTILT 8
#define GAW_ACTION_UPTILT 9
#define GAW_ACTION_FORWARDSMASHHOLD 10
#define GAW_ACTION_FORWARDSMASH 11
#define GAW_ACTION_DOWNSMASHHOLD 12
#define GAW_ACTION_DOWNSMASH 13
#define GAW_ACTION_UPSMASHHOLD 14
#define GAW_ACTION_UPSMASH 15
#define GAW_ACTION_DASHATTACK 18
#define GAW_ACTION_NEUTRALSPECIAL 20
#define GAW_ACTION_SIDESPECIAL 21
#define GAW_ACTION_DOWNSPECIAL 22
#define GAW_ACTION_UPSPECIAL 25
#define GAW_ACTION_PARACHUTE 26
#define GAW_ACTION_LEDGEGRAB 30
#define GAW_ACTION_BACKAIR 40
#define GAW_ACTION_FORWARDAIR 41
#define GAW_ACTION_DOWNAIR 42
#define GAW_ACTION_UPAIR 44
#define GAW_ACTION_NEUTRALAIR 45
#define GAW_ACTION_HURT 50
#define GAW_ACTION_SHIELD 51
#define GAW_ACTION_STUN 52

#define GAW_STAGE_OFFSET 18

protected:
    //  animation config
    const uint8_t charIndex = 1;
    const uint16_t blinkPeriod = 2000;      //  how often gaw changes idle animation

    //  physics config
    const float groundSpeed = 1.85*3;  // pps
    const float airSpeed = 0.9*3;

    const float initialJumpSpeed = 2.0*3;
    const float repeatedJumpSpeed = 2.1*3;
    const float gravityRising = 0.08*7;
    const float gravityFalling = 0.12*7;
    const float maxFallingVelocity = -2.6*3;

    const float airResistance = .2;
    const float maxHorizontalSpeed = 19*3;
    const float groundFriction = 0.6;

    const float DIVerticalSpeed = 0.2 * 3;
    const float DIHorizontalSpeed = 0.2 * 3;
    const float DIKnockbackVerticalSpeed = 0.3 * 3;
    const float DIKnockbackHorizontalSpeed = 0.4 * 3;

    //  standing, resting
    long long lastBlink{0};

    //  up special
    float startY;

    //  down air
    bool overrideMaxVelocity;

    //  dash attack
    long long dashAttackStartTime;

    //  side b
    uint8_t sideBStrength;

    //  down b
    uint8_t bucketCount;
    long long lastBucket;
    long long holdBucketStartTime;
    bool droppingBucket;

    //  neutral b
    float proj_x[4];
    float proj_y[4];
    float proj_xVel[4];
    float proj_yVel[4];
    bool proj_active[4];
    bool proj_mirrored[4];
    uint8_t projectileCount;

public:

    Hurtbox neutralAttack = Hurtbox((short)29, 12, SHAPE_RECTANGLE,
                                    20, 20, 1,
                                    0.8, 0.8, 0, 3);
    Hurtbox dashAttack = Hurtbox((short)0, 15, SHAPE_RECTANGLE,
                                    28, 32, 1,
                                    3, 3.1, 2.5, -1);
    Hurtbox forwardTilt = Hurtbox((short)35, 11, SHAPE_RECTANGLE,
                                  20, 30, 1,
                                  4  , 2.5, 3.5, -1);
    Hurtbox downTilt = Hurtbox((short)24, 8, SHAPE_RECTANGLE,
                                  9, 23, 1,
                                  3, 2.5, 3.7, -1);
    Hurtbox upTilt1 = Hurtbox((short)14, 40, SHAPE_RECTANGLE,
                              23, 23, 1,
                              7, 3.3, 4.1, -1);
    Hurtbox upTilt2 = Hurtbox((short)-21, 43, SHAPE_RECTANGLE,
                              23, 23, 1,
                              7, 3.3, 4.1, -1);
    Hurtbox forwardSmash = Hurtbox((short)30, 18, SHAPE_RECTANGLE,
                                  20, 20, 1,
                                  10, 3.5, 3.8, -1);
    Hurtbox upSmash = Hurtbox((short)1, 20, SHAPE_RECTANGLE,
                              23, 46, 1,
                              10, 3.7, 3.9, -1);
    Hurtbox downSmash = Hurtbox((short)0, 12, SHAPE_RECTANGLE,
                              24, 66, 1,
                              8, 4.0, 3.2, -1);
    Hurtbox forwardAir = Hurtbox((short)24, 20, SHAPE_RECTANGLE,
                                  15, 25, 1,
                                  6, 3.9, 3.5, -1);
    Hurtbox downAir = Hurtbox((short)5, 12, SHAPE_RECTANGLE,
                               26, 14, 1,
                               7, 3.7, 3.1, -1);
    Hurtbox upAir = Hurtbox((short)-4, 44, SHAPE_RECTANGLE,
                              19, 20, 1,
                              3, 2.2, 3.7, -1);
    Hurtbox backAir = Hurtbox((short)-25, 15, SHAPE_RECTANGLE,
                                 21, 31, 1,
                                 8, 3.7, 3.0, -1);
    Hurtbox neutralAir = Hurtbox((short)1, 34, SHAPE_RECTANGLE,
                                30, 40, 1,
                                5, 3.3, 3.3, -1);
    Hurtbox sideSpecial = Hurtbox((short)21, 28, SHAPE_RECTANGLE,
                                  16, 24, 1,
                                  7, 2.8, 2.8, -1);
    Hurtbox neutralSpecialProjectile = Hurtbox(true,0, 5, SHAPE_CIRCLE,
                                 8, 1,
                                 2, 1.2, 1.5, -1);
    Hurtbox downSpecialProjectile = Hurtbox((short)62, 20, SHAPE_RECTANGLE,
                                  40, 90, 1,
                                  7, 4.8, 4.8, -1);

    GameandWatch() {
        hitbox = Hitbox(0, 0, 0, 0);
    }

    //  general control loop
    void controlLoop(float joyH, float joyV, bool btnA, bool btnB, bool shield, class Stage* stage,
                     class HitboxManager* hitboxManager) override; //  called every update

    void updateLastValues(float joyH, float joyV, bool btnA, bool btnB, bool shield) override;

    void collide(class Hurtbox *hurtbox, class Player *otherPlayer) override;

    void reset() override;
};

class Valvano: public Player {

#define VAL_ACTION_RESTING 0
#define VAL_ACTION_RUNNING 1
#define VAL_ACTION_FALLING 2
#define VAL_ACTION_JUMPING 3
#define VAL_ACTION_DOUBLEJUMPING 4
#define VAL_ACTION_CROUCHING 5
#define VAL_ACTION_JAB 6
#define VAL_ACTION_FORWARDTILT 7
#define VAL_ACTION_DOWNTILT 8
#define VAL_ACTION_UPTILT 9
#define VAL_ACTION_DASHATTACK 18
#define VAL_ACTION_NEUTRALSPECIAL 20
#define VAL_ACTION_SIDESPECIAL 21
#define VAL_ACTION_DOWNSPECIAL 22
#define VAL_ACTION_UPSPECIAL 25
#define VAL_ACTION_LEDGEGRAB 30
#define VAL_ACTION_FORWARDAIR 40
#define VAL_ACTION_DOWNAIR 42
#define VAL_ACTION_UPAIR 44
#define VAL_ACTION_NEUTRALAIR 45
#define VAL_ACTION_HURT 50
#define VAL_ACTION_SHIELD 51
#define VAL_ACTION_STUN 52

#define VAL_STAGE_OFFSET 13

protected:
    //  animation config
    const uint8_t charIndex = 2;
    const uint16_t blinkPeriod = 1500;

    //  physics config
    const float groundSpeed = 1.7*3;  // pps
    const float airSpeed = 1.1*3;

    const float initialJumpSpeed = 2.0*3;
    const float repeatedJumpSpeed = 2.1*3;
    const float gravityRising = 0.08*7;
    const float gravityFalling = 0.12*7;
    const float maxFallingVelocity = -2.6*3;

    const float airResistance = .2;
    const float maxHorizontalSpeed = 19*3;
    const float groundFriction = 0.6;

    const float DIVerticalSpeed = 0.2 * 3;
    const float DIHorizontalSpeed = 0.2 * 3;
    const float DIKnockbackVerticalSpeed = 0.3 * 3;
    const float DIKnockbackHorizontalSpeed = 0.4 * 3;

    //  standing, resting
    long long lastBlink{0};

    //  up special
    long long upSpecialStartTime;

    //  dash attack
    long long dashAttackStartTime;

    //  side b

    //  down b
    bool robotCarActive, robotMirrored, robotCarActivationFlag;
    long long robotCarStartTime;
    float robotX, robotY, robotYVel;
    int8_t robotCarFrameIndex, robotCarFrameCounter;

    //  neutral b
    bool laserActive, laserMirrored, laserActivationFlag;
    long long laserStartTime;
    float laserX, laserY;

public:
    Hurtbox jab = Hurtbox(true,13, 14, SHAPE_CIRCLE,
                                               5, 1,
                                               2, 1.9, 1.5, 2);
    Hurtbox dashAttack = Hurtbox((short)21, 17, SHAPE_RECTANGLE,
                                 34, 10, 1,
                                 3, 3.1, 2.5, -1);
    Hurtbox forwardTilt1 = Hurtbox((short)16, 25, SHAPE_RECTANGLE,
                                   12, 18, 1,
                                   4, 2.5, 3.5, -1);
    Hurtbox forwardTilt2 = Hurtbox((short)26, 11, SHAPE_RECTANGLE,
                                   14, 16, 1,
                                   4, 2.5, 3.5, -1);
    Hurtbox forwardTilt3 = Hurtbox((short)32, 7, SHAPE_RECTANGLE,
                                   14, 12, 1,
                                   4, 2.5, 3.5, -1);
    Hurtbox downTilt = Hurtbox((short)0, 9, SHAPE_RECTANGLE,
                               7, 32, 1,
                               3, 2.5, 3.7, -1);
    Hurtbox upTilt1 = Hurtbox((short)14, 40, SHAPE_RECTANGLE,
                              23, 23, 1,
                              7, 3.3, 4.1, -1);
    Hurtbox upTilt2 = Hurtbox((short)-21, 43, SHAPE_RECTANGLE,
                              23, 23, 1,
                              7, 3.3, 4.1, -1);
    Hurtbox forwardAir =  Hurtbox(true,25, 14, SHAPE_CIRCLE,
                                  11, 1,
                                  2, 1.9, 1.5, 2);
    Hurtbox downAir = Hurtbox(true,0, 0, SHAPE_CIRCLE,
                              11, 1,
                              2, 1.9, 1.5, 2);
    Hurtbox upAir = Hurtbox((short)-1, 41, SHAPE_RECTANGLE,
                            21, 20, 1,
                            3, 2.2, 3.7, -1);
    Hurtbox neutralAir = Hurtbox((short)0, 22, SHAPE_RECTANGLE,
                                 42, 50, 1,
                                 5, 3.3, 3.3, -1);
    Hurtbox sideSpecial1 = Hurtbox((short)13, 20, SHAPE_RECTANGLE,
                                   5, 16, 1,
                                   7, 2.8, 2.8, -1);
    Hurtbox sideSpecial2 = Hurtbox((short)26, 17, SHAPE_RECTANGLE,
                                   6, 18, 1,
                                   7, 2.8, 2.8, -1);
    Hurtbox sideSpecial3 = Hurtbox((short)33, 17, SHAPE_RECTANGLE,
                                   6, 48, 1,
                                   7, 2.8, 2.8, -1);
    Hurtbox sideSpecial4 = Hurtbox((short)25, 17, SHAPE_RECTANGLE,
                                   8, 36, 1,
                                   7, 2.8, 2.8, -1);
    Hurtbox laserProjectile = Hurtbox((float)0., 1, SHAPE_RECTANGLE,
                                      3, 15, 1,
                                      3.5, 1.5, 1.6, 3,
                                      true, &laserActivationFlag);
    Hurtbox robotCar = Hurtbox((float)0., 5, SHAPE_RECTANGLE,
                                            10, 16, 1,
                                            11, 3.9, 4.1, -1,
                                            true, &robotCarActivationFlag);

    Valvano() {
        hitbox = Hitbox(0, 0, 0, 0);
    }

    //  general control loop
    void controlLoop(float joyH, float joyV, bool btnA, bool btnB, bool shield, class Stage* stage,
                     class HitboxManager* hitboxManager) override; //  called every update

    void updateLastValues(float joyH, float joyV, bool btnA, bool btnB, bool shield) override;

    void collide(class Hurtbox *hurtbox, class Player *otherPlayer) override;

    void reset() override;
};


#endif //EE319K_FINAL_PROJECT_INITIAL_TESTING_ENTITIES_H
