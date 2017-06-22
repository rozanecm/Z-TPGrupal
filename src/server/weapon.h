//
// Created by rodian on 21/05/17.
//

#ifndef Z_TPGRUPAL_WEAPON_H
#define Z_TPGRUPAL_WEAPON_H

#include <iostream>
#include <vector>
#include "size.h"
#include "Occupant.h"
#include "bullet.h"

class Weapon {
private:
    std::string type;
    int damage, w_speed;
    bool explosive;
    Size w_size;
    Bullet* bullet;

public:
    Weapon(std::string& type, int damage, int w_speed, bool explosive,
                                                        Size w_size);

    void setNewTarget(Occupant* target);

    Bullet* shotTarget(Occupant* target);

    bool isTheAttackExplosive();

    std::vector<Position>& getBulletRoad();

    Size getBulletSize() const;

    void recalculateRoadToTarget();

    void movePosition(int x,int y);
};


#endif //Z_TPGRUPAL_WEAPON_H
