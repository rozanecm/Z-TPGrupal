//
// Created by rodian on 22/05/17.
//

#ifndef Z_TPGRUPAL_BULLET_H
#define Z_TPGRUPAL_BULLET_H

#include <string>
#include "size.h"
#include "Occupant.h"

class Bullet {
private:
    std::string type;
    int damage, w_speed;
    Size w_size;
    bool hit;

public:
    Bullet(std::string type, int damage, int w_speed, Size w_size);

    // Pursues the Target in straight line on each TIC til hits
    void shotTarget(Occupant* target);

    bool didHit();
};


#endif //Z_TPGRUPAL_BULLET_H
