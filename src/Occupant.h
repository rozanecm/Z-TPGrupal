#ifndef Z_TPGRUPAL_OCCUPANT_H
#define Z_TPGRUPAL_OCCUPANT_H

#include <iostream>
#include "cell.h"
// Class Cell later defined
class Cell;

// Class Occupant so any object knows where is on the map
class Occupant {
    private:
        Cell* position;
        int id;
    public:
        // Constructor for Occupant on a specific Cell and it's id
        Occupant(Cell* position, int id);

        // Constructor for Occupant with it's id. The position must be set
        // later with setNewPosition method before using getPosition method
        Occupant(int id);

        // Returns the id of the object
        int getId() const;

        // Indicates the Occupant it's new position
        void setNewPosition(Cell* position);

        // Returns the cell
        Cell* getPosition();

        ~Occupant();
};


#endif //Z_TPGRUPAL_OCCUPANT_H
