//
// Created by rodian on 21/05/17.
//

#include "weapon.h"

Weapon::Weapon(std::string type, int damage, int w_speed,
               bool explosive, Size w_size) : type(type), damage(damage),
                w_speed(w_speed), explosive(explosive), w_size(w_size) {}

void Weapon::shotTarget(Occupant *target) {
    // while(didnt_hit)
    // calculate straight line to target
    // make step
}
