//
// Created by rodian on 21/05/17.
//

#include "weapon.h"

Weapon::Weapon(std::string& type, int dmg, int w_speed,
               bool explosive, Size w_size) : type(type), damage(dmg),
                w_speed(w_speed), explosive(explosive), w_size(w_size),
                bullet(type,dmg,w_speed,w_size) {}

Bullet* Weapon::shotTarget(Occupant* target) {
    Bullet* shoted_bullet = new Bullet(type,damage,w_speed,w_size,target);
    shoted_bullet->shotTarget(target);
    return shoted_bullet;
}

bool Weapon::isTheAttackExplosive() {
    return explosive;
}

void Weapon::setNewTarget(Occupant* target) {
    this->bullet.shotTarget(target);
}

std::vector<Position>& Weapon::getBulletRoad() {
    return this->bullet.getRoad();
}

Size Weapon::getBulletSize() const {
    return this->w_size;
}

void Weapon::recalculateRoadToTarget() {
    this->bullet.calculateRoadToTarget();
}

void Weapon::movePosition(int x, int y) {
    w_size.moveTo(x,y);
    this->bullet.setStartLocation(x,y);
}

