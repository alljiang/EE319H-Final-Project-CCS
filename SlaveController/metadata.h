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

#define CHARACTER_KIRBY 0
#define CHARACTER_GAMEANDWATCH 1
#define CHARACTER_VALVANO 2

#define STAGE_FINALDESTINATION 0
#define STAGE_TOWER 1
#define STAGE_BATTLEFIELD 2
#define STAGE_SMASHVILLE 3
#define STAGE_EER 4
#define STAGE_GREGORYGYM 5

#define BACKGROUND_FINALDESTINATION 0
#define BACKGROUND_TOWER 1
#define BACKGROUND_BATTLEFIELD 2
#define BACKGROUND_SMASHVILLE 3
#define BACKGROUND_EER 4
#define BACKGROUND_GREGORYGYM 5
#define BACKGROUND_MENU 10
#define BACKGROUND_STAGEMENU 11
#define BACKGROUND_WIN_P1_KIRBY 14
#define BACKGROUND_WIN_P2_KIRBY 15
#define BACKGROUND_WIN_P1_GAMEANDWATCH 16
#define BACKGROUND_WIN_P2_GAMEANDWATCH 17
#define BACKGROUND_WIN_P1_VALVANO 18
#define BACKGROUND_WIN_P2_VALVANO 19

#define CHARACTERS  5

const char characterNames[CHARACTERS][14] = {
        {"kirby"},
        {"gaw"},
        {"val"},    //  TODO: CHANGE THIS BACK TO SHORTER "VAL" WHEN PORTING TO TM4C
        {"misc"},
        {"menu"}
};

const char persistentSprites[20][15] = {
/* 0  */{"fdst"},
        {"tower"},
        {"battl"},
        {"smvl"},
        {"eer"},
        {"greg"},
        {""},
        {""},
        {""},
        {""},
/* 10 */{"cmenu"},
        {"smenu"},
        {""},
        {""},
        {"w1kb"},
        {"w2kb"},
        {"w1gw"},
        {"w2gw"},
        {"w1va"},
        {"w2va"}
};

const uint16_t numberOfAnimations = 64;
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
                                                {"starproj"},   //  1 + 3
                                                {"neutralb"},    //  4
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
                                                {"shield"},     //  1
                                                {""},
        },
        {
                /*  0   */                      {"rest"},   //  1 + 1
                                                {"walk"},   //  1 (start) + 2
                                                {"dash"},   //  2
                                                {"jump"},   //  4
                                                {"djump"},  //  4
                                                {"fall"},   //  2
                                                {"crouch"}, //  1
                                                {""},
                                                {""},
                                                {""},
                /*  10  */                      {"ftilt"},  //  2
                                                {"utilt"},  //  3
                                                {"dtilt"},  //  4
                                                {"dashatk"},    //  1
                                                {"fsmash"},     //  2 (charge) + 2
                                                {"dsmash"},     //  2 (charge) + 1
                                                {"usmash"},     //  2 (charge) + 2
                                                {"neuatk"},     //  2
                                                {""},
                                                {""},
                /*  20  */                      {"nair"},   //  3
                                                {"fair"},   //  3
                                                {"bair"},   //  3
                                                {"dair"},   //  3
                                                {"uair"},   //  5
                                                {""},
                                                {""},
                                                {""},
                                                {""},
                                                {""},
                /*  30  */                      {"neub"},   //  3
                                                {"neubproj"},   //  4 (each frame is another projectile)
                                                {"sideb"},  //  3
                                                {"sidebnum"},   //  9 (each frame is a number, frame 0 = 1)
                                                {"downb"},  //  1 (catch) + 1 (let go) + 3 (charge levels) + 1 (release)
                                                {"downbproj"},  //  1
                                                {"upb"},    //  2 (up) + 1 (peak) + 2 (down)
                                                {"upbfall"},    //  1 (start) + 8 (loop)
                                                {""},
                                                {""},
                /*  40  */                      {"hurt"},   //  2
                                                {"shield"},     //  1
                                                {"ledge"},      //  1
                                                {"stun"}    //  4

        },
        {
                /*  0   */                      {"rest"},   //  1 + 3 (blink)
                                                {"crouch"}, //  1
                                                {"run"},    //  5
                                                {"jump"},   //  1
                                                {"falling"},    //  2
                                                {""},
                                                {""},
                                                {""},
                                                {""},
                                                {""},
               /*   10  */                      {"dashattack"},     //  5
                                                {"jab"},    //  5 (first jab) + 3 (second jab)
                                                {""},
                                                {""},
                                                {""},
                                                {""},
                                                {""},
                                                {""},
                                                {""},
                                                {""},
               /*   20  */                      {"nair"},   //  4
                                                {"fair"},   //  6
                                                {"dair"},   //  2
                                                {""},
                                                {""},
                                                {""},
                                                {""},
                                                {""},
                                                {""},
                                                {""},
               /*   30  */                      {"neutralb"},   //  3
                                                {"sidespecial"},    //  8
                                                {"upspecial"},      //  5
                                                {"laserproj"},  //  1
                                                {"downspecial"},    //  2
                                                {"robotcar"},   //  2 + 3 (explosion)
                                                {""},
                                                {""},
                                                {""},
                                                {""},
               /*   40  */                      {""},
                                                {""},
                                                {""},
                                                {""},
                                                {""},
                                                {""},
                                                {""},
                                                {""},
                                                {""},
                                                {""},

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
                                                {"321go"},      //  36
                /*  10  */                      {"game"},       //  25
                                                {"shield"},      //  3 (red) + 3 (blue)
                                                {""},
                                                {""},
                                                {""},
                                                {""},
                                                {""},
                                                {""},
                                                {""},
                                                {""},
                /*  20  */                      {"platform"},
                                                {""},
                                                {""},
                                                {""},
                                                {""},
                                                {""},

        },
        {
                /*  0   */                      {"p1small"},
                                                {"p1big"},
                                                {"p2small"},
                                                {"p2big"},
                                                {"stageselect"},
                                                {""},
                                                {""},
                                                {""},
                                                {""},
                                                {""},
               /*   10  */                      {"prvwkirby"},
                                                {"prvwvalvano"},
                                                {"prvwgaw"},
                                                {""},
                                                {""},
                                                {""},
                                                {""},
                                                {""},
                                                {""},
                                                {""},
                                                {""},
                                                {""},
        }
};

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
