//
// Created by rodian on 21/05/17.
//

#include "weapon.h"

Weapon::Weapon(std::string type, int damage, int w_speed,
               bool explosive, Size w_size) : type(type), damage(damage),
                w_speed(w_speed), explosive(explosive), w_size(w_size) {}

Bullet* Weapon::shotTarget(Occupant& target) {
    bullets.back()->shotTarget(target);
}

bool Weapon::isTheAttackExplosive() {
    return explosive;
}

void Weapon::setNewTarget(Occupant &target) {
    bullets.emplace_back(new Bullet(type,damage,w_speed,w_size,target));
}

std::vector<Position> *Weapon::getBulletRoad() {
    return bullets.back()->getRoad();
}

Size Weapon::getBulletSize() const {
    return this->w_size;
}

void Weapon::recalculateRoadToTarget() {
    bullets.back()->calculateRoadToTarget();
}

void Weapon::movePosition(int x, int y) {
    w_size.moveTo(x,y);
}
