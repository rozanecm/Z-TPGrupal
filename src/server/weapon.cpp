//
// Created by rodian on 21/05/17.
//

#include "weapon.h"

Weapon::Weapon(std::string type, int damage, int w_speed,
               bool explosive, Size w_size) : type(type), damage(damage),
                w_speed(w_speed), explosive(explosive), w_size(w_size) {}

void Weapon::shotTarget(Occupant& target) {
    bullets.emplace_back(type,damage,w_speed,w_size,target);

    bullets.back().shotTarget(target);

    this->checkIfBullestHit();
}

bool Weapon::isTheAttackExplosive() {
    return explosive;
}

void Weapon::checkIfBullestHit() {
//    std::vector<Bullet>::iterator it = bullets.begin();
//    while (it != bullets.end()) {
//        if (it->didHit()) {
//            bullets.erase(it);
//        } else {
//            ++it;
//        }
//    }
}
