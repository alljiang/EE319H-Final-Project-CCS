//

// Created by Allen on 2/5/2020.
//

#ifndef METADATA
#define METADATA

#include <stdbool.h>

/*
 *  Note: if the sprite is persistent, then the layer will be ignored when painting the background, so send them in the right order!
 */

#define LAYER_NULL 0
#define LAYER_BOTTOM 1
#define LAYER_BACKGROUND 10
#define LAYER_BACKGROUND_ANIMATION 20
#define LAYER_CHARACTER 30
#define LAYER_CHARACTER_ATTACK 40
#define LAYER_NAMETAG 50
#define LAYER_CHARACTER_PROJECTILE 60
#define LAYER_OVERLAY 100
#define LAYER_PERCENTAGE 110
#define LAYER_TOP 255

#define STAGE_FINALDESTINATION 0
#define STAGE_TOWER 1
#define STAGE_BATTLEFIELD 2

#define CHARACTERS  4

const char characterNames[CHARACTERS][15] = {
        {"kirby"},
        {"marth"},
        {"jigglypuff"},
        {"misc"}
};

const char persistentSprites[4][10] = {
        {"fdst"},
        {"tower"},
        {"battle"},
        {}
};

const char animations[CHARACTERS][64][12] = {
        {                                                   // frames
                /* 0 */                         {"crouch"}, // 1
                                                {"dash"},   // 8
                                                {"fall"},   // 2
                                                {"fallnojmp"},  // 2
                                                {"firstjmp"},   // 8 (use 7 bc staggered)
                                                {"multijmp"},   // 4 (use 3)
                                                {"rest"},   // 7
                                                {"slow"},   // 5
                                                {"slowstop"},   // 2
                                                {"walk"},    //  12
                /* 10 */                        {"jab"},     //  3(single)+2(doub)+(1+6)(repeat)
                                                {"fair"},   //  8
                                                {"fsmash"},     //  10
                                                {"fspecial"},   //  11
                                                {"ftilt"},      //  8
                                                {"downtilt"},   //  4
                                                {"uptilt"},     //  7
                                                {"upsmash"},     //  8
                                                {"ledge"},      //  1
                                                {"upbproj"},    //  1   up special landing projectile
                /* 20 */                        {"upbinit"},    //  3
                                                {"upbrise"},    //  1
                                                {"upbtop"},     //  6
                                                {"upbfall"},    //  1(fall)+1(followthrough)
                                                {"backair"},    //  9
                                                {"downair"},    //  11
                                                {"fwdair"},     //  8
                                                {"upair"},      //  6
                                                {"nair"},       //  8
                                                {"dashatk"},    //  7
                /* 30 */                        {""},
                                                {""},
                                                {"neutralb"},    //  11
                                                {"fwdspec"},   //  10
                                                {"downspec"},   //  3
                                                {"downsmash"},  //  12
                                                {""},
                                                {""},
                                                {""},
                                                {""},
                /* 40 */                        {"hit"},        //  3
                                                {"hurt"},       //  8
                                                {"hurtfall"},   //  3
                                                {"knockout"},   //  1
                                                {"stun"},       //  8
                                                {""},
                                                {""},
        },
        {

        },
        {

        },
        {
                /*  0   */                      {"num0"},       //  1
                                                {"num1"},       //  1
                                                {"num2"},       //  1
                                                {"num3"},       //  1
                                                {"numdot"},     //  1
                                                {"blastdown"},  //  17
                                                {"blastleft"},  //  17
                                                {"blastright"}, //  17
                                                {"blastup"},    //  17

        }
};

const uint16_t numberOfAnimations = 100;

struct Animation {
    uint32_t memLocation;
    uint8_t width;
    uint8_t height;
    uint8_t frames;
    uint8_t characterIndex;
    uint8_t animationIndex;
};

struct SpriteSendable {
    int16_t x;
    int16_t y;
    uint8_t charIndex;
    uint8_t animationIndex;
    uint8_t frame;      //  current frame of animation
    bool persistent;        //  if the animation is persistent,
    uint8_t layer;       //  priority for layering over other sprites. high layer value = top layer
    bool continuous;    //  continue to next frame of animation
    uint8_t framePeriod;    //  how many updates each frame lasts
    uint8_t currentframeLength;
    bool mirrored;
};

#endif //METADATA
