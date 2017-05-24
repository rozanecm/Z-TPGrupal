#ifndef Z_TPGRUPAL_CELL_H
#define Z_TPGRUPAL_CELL_H

#include "lock.h"
#include "Terrain.h"
#include "size.h"
#include <iostream>
#include <string>
#include <cstdbool>
#include <mutex>

// Class Cell to represent a position on the map
class Cell {
    private:
        Terrain terrain;
        Size size;

    public:
        // Cell constructor on position (x,y). Builds the Terrain inside it and
        // always has an Occupant. The empty Cell will be the one who has an
        // Occupant with id = -1.
        Cell(int x, int y, int width, int lenght,
                std::string& kind, int factor);

        // Returns a string with the king of Terrain
        std::string getTerrainType() const;

        // Returns the movement factor of the Terrain
        double getMovementFactor() const;

        Position getPosition() const;

        int getWidthOfCell();

        bool areYouOnThisPosition(int x_pos, int y_pos);

    bool isThereACollision(Size& size);

        ~Cell();
};

#endif
