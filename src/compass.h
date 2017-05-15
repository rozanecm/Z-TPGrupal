//
// Created by rodian on 14/05/17.
//

#ifndef Z_TPGRUPAL_COMPASS_H
#define Z_TPGRUPAL_COMPASS_H

#include <iostream>
#include <vector>
#include "cell.h"

// class Compass so every moving unit knows the fastest way to destiny
class Compass {
    private:
    const std::vector<std::vector<Cell>> map;
    int unit_speed;

    public:
        // The Compass receives the map of Cells for calculations and the
        // basic unit speed
        Compass(std::vector<std::vector<Cell>>& map, int unit_speed);

        // Recieves the current position of the unit and the destiny
        // Returns a vector of Cells with the fastest way
        std::vector<Cell> getFastestWay(Cell& from, Cell& to);

        ~Compass();

    private:
        // Sets the H value for every Cell
};


#endif //Z_TPGRUPAL_COMPASS_H
