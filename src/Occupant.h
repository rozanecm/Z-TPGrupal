#ifndef Z_TPGRUPAL_OCCUPANT_H
#define Z_TPGRUPAL_OCCUPANT_H

#include <iostream>
#include "cell.h"
#include "size.h"
#include "map.h"

// Class Cell later defined
class Cell;

// Class Occupant so any object knows where is on the map
class Occupant {
protected:
    int id, life_points;
    Size occ_size;
    Map wordl_map;

public:
    // Constructor for Occupant on a specific position saved in Size and it's id
    Occupant(int life, int id, Size position, Map map);

    // Returns the id of the object
    int getId() const;

    void reduceLifeBy(int dmg);

    // Returns the
    int getXPosition();

    int getYPosition();

    bool areYouAlive();

    ~Occupant();
};


#endif //Z_TPGRUPAL_OCCUPANT_H
