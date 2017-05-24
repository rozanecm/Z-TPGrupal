//
// Created by rodian on 22/05/17.
//

#include "bullet.h"

Bullet::Bullet(std::string type, int dmg, int w_speed, Size w_size) :
        type(type), damage(dmg), w_speed(w_speed), w_size(w_size), hit(false) {}

void Bullet::shotTarget(Occupant *target) {
    Position target_pos = target->getPosition();
    // while(didn't hit)
    // Calculate strait line
    // move couple of steps
    // sleep

    hit = true;
}

bool Bullet::didHit() {
    return hit;
}
