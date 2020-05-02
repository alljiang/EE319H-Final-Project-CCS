
#ifndef EE319K_FINAL_PROJECT_INITIAL_TESTING_STAGES_H
#define EE319K_FINAL_PROJECT_INITIAL_TESTING_STAGES_H

#include <cstdint>
#include "entities.h"

class Stage {

#define CEIL_MAX 500
#define FLOOR_MAX -200
#define RIGHT_MAX 500
#define LEFT_MAX -200

#define SMASHVILLE_PLATFORM_ACCELERATON 0.07
#define SMASHVILLE_PLATFORM_MAXSPEED 2.0
#define SMASHVILLE_PLATFORM_DECELPOINTLEFT 50
#define SMASHVILLE_PLATFORM_DECELPOINTRIGHT 195
#define SMASHVILLE_PLATFORM_LENGTH 78
#define SMASHVILLE_PLATFORM_HEIGHT 5

protected:
    double sv_platform_x, sv_platform_y, sv_platform_xvel;
    bool sv_platform_goingRight;

public:
    uint8_t stageIndex;

    void initialize(uint8_t index, HitboxManager *hitboxManager);
    void update();

    double ceil(double x, double y);
    double floor(double x, double y);
    double rightBound(double x, double y);
    double leftBound(double x, double y);

    double getStartX(uint8_t player);
    double getStartY(uint8_t player);

    bool onPlatform(double x, double y);
    double xVelocity(double x, double y);

};

#endif //EE319K_FINAL_PROJECT_INITIAL_TESTING_STAGES_H
