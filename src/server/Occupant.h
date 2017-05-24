#ifndef Z_TPGRUPAL_OCCUPANT_H
#define Z_TPGRUPAL_OCCUPANT_H

#include <iostream>
#include "size.h"


// Class Occupant so any object knows where is on the map
class Occupant {
protected:
    int life_points, damage_recv;
    Size occ_size;
    std::string type;

public:
    // Constructor for Occupant on a specific position saved in Size and it's id
    Occupant(int life, std::string type, Size occ_size);

    // Returns the id of the object
    std::string getType() const;

    void reduceLifeBy(int dmg);

    // Returns the
    Position getPosition() const;

    bool areYouAlive();

    bool isThereACollision(Size& size);

    int getLifeLeft();

    ~Occupant();
};


#endif //Z_TPGRUPAL_OCCUPANT_H
