//
// Created by rodian on 21/05/17.
//

#ifndef Z_TPGRUPAL_WEAPON_H
#define Z_TPGRUPAL_WEAPON_H

#include <iostream>
#include "size.h"
#include "Occupant.h"

class Weapon {
private:
    std::string type;
    int damage, w_speed;
    bool explosive;
    Size w_size;

public:
    Weapon(std::string type, int damage, int w_speed, bool explosive,
                                                        Size w_size);

    void shotTarget(Occupant* target);
};


#endif //Z_TPGRUPAL_WEAPON_H
