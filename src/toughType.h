//
// Created by rodian on 21/05/17.
//

#ifndef Z_TPGRUPAL_TOUGHTYPE_H
#define Z_TPGRUPAL_TOUGHTYPE_H

#include <iostream>
#include "unit.h"
#include "weapon.h"

// falta ver tema del arma

class ToughType {
private:
    int tec_level, life, range, width, height, unit_speed, fire_rate,
            creation_time;

public:
    ToughType(int tec_level, int life, int range, int width, int height,
              int unit_speed, int fire_rate, int creation_time);

    Unit* createTought(int x, int y, Map& map);

    // Returns the technology level needed to create Tough unit
    int getTechnologyLevel() const;

    // Returns creation time in seconds
    int getCreationTime();

};


#endif //Z_TPGRUPAL_TOUGHTYPE_H
