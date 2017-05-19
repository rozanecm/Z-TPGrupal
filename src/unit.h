//
// Created by rodian on 18/05/17.
//

#ifndef Z_TPGRUPAL_UNIT_H
#define Z_TPGRUPAL_UNIT_H

#include "Occupant.h"
#include "compass.h"
//#include "map.h"

class Map;

class Unit: public Occupant {
private:
    Compass compass;
//    Size unit_size;
public:
    Unit(int life, int id, Size size, Map& map);

    // Indicates the Unit it's new position
    void setNewPosition(int x, int y);

};


#endif //Z_TPGRUPAL_UNIT_H
