
#ifndef EE319K_FINAL_PROJECT_INITIAL_TESTING_STAGES_H
#define EE319K_FINAL_PROJECT_INITIAL_TESTING_STAGES_H

#include <cstdint>
#include "entities.h"

class Stage {

#define CEIL_MAX 500
#define FLOOR_MAX -200
#define RIGHT_MAX 500
#define LEFT_MAX -200

protected:
    uint8_t stageIndex;

public:
    void initialize(uint8_t index, HitboxManager *hitboxManager);
    void update();

    double ceil(double x, double y);
    double floor(double x, double y);
    double rightBound(double x, double y);
    double leftBound(double x, double y);

    double getStartX(uint8_t player);
    double getStartY(uint8_t player);

    double xVelocity(double x, double y);

};

#endif //EE319K_FINAL_PROJECT_INITIAL_TESTING_STAGES_H
