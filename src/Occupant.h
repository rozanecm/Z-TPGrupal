#ifndef Z_TPGRUPAL_OCCUPANT_H
#define Z_TPGRUPAL_OCCUPANT_H

#include <iostream>
#include "cell.h"
// Class Cell later defined
class Cell;

// Class Occupant so any object knows where is on the map
class Occupant {
private:
    int id, life_points;
    Size position;

public:
    // Constructor for Occupant on a specific position saved in Size and it's id
    Occupant(int life, int id, Size position);

    // Returns the id of the object
    int getId() const;

    // Indicates the Occupant it's new position
    void setNewPosition(int x, int y);

    void reduceLifeBy(int dmg);

    // Returns the
    int getXPosition();

    int getYPosition();

    bool areYouAlive();

    ~Occupant();
};


#endif //Z_TPGRUPAL_OCCUPANT_H
