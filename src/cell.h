#ifndef Z_TPGRUPAL_CELL_H
#define Z_TPGRUPAL_CELL_H

#include "Terrain.h"
#include "Occupant.h"
#include <iostream>
#include <string>
#include <cstdbool>
// Class Occupant later defined
class Occupant;

// Class Cell to represent a position on the map
class Cell {
    private:
        Terrain terrain;
        Occupant* occupant;
        int x;
        int y;

    public:
        // Cell constructor on position (x,y). Builds the Terrain inside it and
        // always has an Occupant. The empty Cell will be the one who has an
        // Occupant with id = -1.
        Cell(int x, int y, std::string& kind, int factor,  Occupant* ocpt);

        // Returns true if it's empty. If not, false
        bool isEmpty() const;

        std::string getTerrainType() const;

        int getMovementFactor() const;

        // Returns the object on the cell.
        Occupant* getOccupant();

        ~Cell();
};

#endif
