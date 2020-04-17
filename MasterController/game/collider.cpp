//
// Created by Allen on 3/2/2020.
//

#include <cstdlib>
#include <cstdio>
#include "entities.h"
#include "utils.h"

using namespace std;

void HitboxManager::checkCollisions() {
    for (uint8_t slot = 0; slot < hurtboxSlots; slot++) {
        if(!hurtboxes[slot].active) continue;
        if(p2 != nullptr && hurtboxes[slot].source == 1 && p2->hitbox.isColliding(hurtboxes[slot])) {
            //  hurtbox collision with player 2!
            hurtboxes[slot].active = false;
            p2->collide(&hurtboxes[slot], p1);
        }
        else if(p2 != nullptr && hurtboxes[slot].source == 2 && p1->hitbox.isColliding(hurtboxes[slot])) {
            //  hurtbox collision with player 1!
            hurtboxes[slot].active = false;
            p1->collide(&hurtboxes[slot], p2);
        }
        else if(hurtboxes[slot].source == 0) {
            if(p1->hitbox.isColliding(hurtboxes[slot])) {
                p1->collide(&hurtboxes[slot], p2);
            }
            if(p2 != nullptr && p2->hitbox.isColliding(hurtboxes[slot])) {
                p2->collide(&hurtboxes[slot], p1);
            }
        }
        else {
            //  update hurtbox frame
            if(!((persistentHurtbox >> slot) & 1)) {
                if(++hurtboxes[slot].currentFrame >= hurtboxes[slot].frames) {
                    hurtboxes[slot].active = false;
                }
            }
        }
    }
}

void HitboxManager::addHurtbox(double xOffset, double yOffset, bool mirrored,
        class Hurtbox hurtBox, uint8_t playerSource, bool persistent) {
    hurtBox.active = true;
    hurtBox.currentFrame = 0;

    if(mirrored) hurtBox.x = xOffset-hurtBox.x;
    else hurtBox.x += xOffset;
    hurtBox.y += yOffset;
    hurtBox.setSource(playerSource);

    //  add the hurtbox to the array, find a slot for it
    uint8_t slot = 0;
    for(slot = 0; slot < hurtboxSlots; slot++) {
        if(!hurtboxes[slot].active) {
            if(persistent) persistentHurtbox |= 1u << slot;
            hurtboxes[slot] = hurtBox;
            break;
        }
    }
    if(slot == hurtboxSlots) return;    //  no slots remaining
}

void HitboxManager::addHurtbox(double xOffset, double yOffset, bool mirrored,
       class Hurtbox hurtBox, uint8_t playerSource) {
    this->addHurtbox(xOffset, yOffset, mirrored, hurtBox, playerSource, false);
}

bool Hitbox::isColliding(class Hurtbox hurtbox) {
    double thisx = x + xOffset;
    double thisy = y + yOffset;
    double thisradius = radius + radiusOffset;

    double hbx = hurtbox.x + hurtbox.xOffset;
    double hby = hurtbox.y + hurtbox.yOffset;
    double hbh = hurtbox.height;
    double hbw = hurtbox.width;
    double hbr = hurtbox.radius;

    switch (this->shape) {
        case SHAPE_CIRCLE:
            if(hurtbox.shape == SHAPE_CIRCLE) {
                /*
                 * Find the distance between the two circles.
                 * If this distance is less than the sum of the radii
                 * of the circles, they're colliding.
                 */
                return absVal((thisx - hbx) * (thisx - hbx)
                + (thisy - hby) * (thisy - hby))
                < (thisradius + hbr) * (thisradius + hbr);
            }
            else if(hurtbox.shape == SHAPE_RECTANGLE) {
                double distanceX = absVal(thisx - hbx);
                double distanceY = absVal(thisy - hby);

                /*
                 * If the distance between the two objects are greater
                 * than the radius and the distance from center to edge combined,
                 * there is definitely no collision
                 */
                if (distanceX > (hbw/2 + thisradius)) { return false; }
                if (distanceY > (hbh/2 + thisradius)) { return false; }

                /*
                 * If the center of the circle is inside the rectangle,
                 * there's a collision.
                 */
                if (distanceX <= (hbw/2)) { return true; }
                if (distanceY <= (hbh/2)) { return true; }

                /*
                 * Check if the circle intersects rectangle corner
                 */
                double cornerDistance_sq = (distanceX - hbw/2)*(distanceX - hbw/2) +
                                            (distanceY - hbh/2)*(distanceY - hbh/2);

                return (cornerDistance_sq <= (this->radius*thisradius));
            }
        case SHAPE_RECTANGLE:
            if(hurtbox.shape == SHAPE_CIRCLE) {
                double distanceX = absVal(thisx - hbx);
                double distanceY = absVal(thisy - hby);

                /*
                 * If the distance between the two objects are greater
                 * than the radius and the distance from center to edge combined,
                 * there is definitely no collision
                 */
                if (distanceX > (this->width/2 + hbr)) { return false; }
                if (distanceY > (this->height/2 + hbr)) { return false; }

                /*
                 * If the center of the circle is inside the rectangle,
                 * there's a collision.
                 */
                if (distanceX <= (this->width/2)) { return true; }
                if (distanceY <= (this->height/2)) { return true; }

                /*
                 * Check if the circle intersects rectangle corner
                 */
                double cornerDistance_sq =
                        (distanceX - this->width/2)*(distanceX - this->width/2) +
                        (distanceY - this->height/2)*(distanceY - this->height/2);

                return (cornerDistance_sq <= (hbr*hbr));
            }
            else if(hurtbox.shape == SHAPE_RECTANGLE) {
                return  thisx + this->width >= hbx &&     // r1 right edge past r2 left
                        thisx <= hbx + hbw &&             // r1 left edge past r2 right
                        thisy + this->height >= hby &&    // r1 top edge past r2 bottom
                        thisy <= hby + hbh;               // r1 bottom edge past r2 top
            }
    }
    return false;
}
