
#include <stdint.h>

const int32_t colors[] = {-1,9987429,14326448,16234959,16633064,4215765,15132390,3552822,13123886,16340024,9643805,16776960};

const char sounds[100][10] = {
                                        {"menu"},
                                        {"321go"},
                                        {},
                                        {},
                                        {},
                                        {},
                                        {},
                                        {},
                                        {},
                                        {}
};

const char characterNames[4][15] = {
                                        {"kirby"},
                                        {"marth"},
                                        {"jigglypuff"},
                                        {"gameandwatch"},
};

const char animations[4][64][10] = {
                                        {
                                            {"crouch"},
                                            {"dash"},
                                            {"fall"},
                                            {"fallnojmp"},
                                            {"firstjmp"},
                                            {"multijmp"},
                                            {"rest"},
                                            {"slow"},
                                            {"slowstop"},
                                            {"walk"}
                                        },
                                        {

                                        },
                                        {

                                        },
                                        {

                                        },
};

const uint16_t numberOfAnimations = 100;

struct Animation {
    uint16_t memLocation;
    uint8_t width;
    uint8_t height;
    uint8_t frames;
    uint8_t characterIndex;
    uint8_t animationIndex;
};

