//
// Created by rodian on 14/05/17.
//

#include "compass.h"

Compass::Compass(std::vector<std::vector<Cell>> &map, int unit_speed):
                                    map(map), unit_speed(unit_speed) {}

std::vector<Cell> Compass::getFastestWay(Cell &from, Cell &to) {
    std::vector<Cell> road;
    // set H value for destiny

    // start algorithm
        // add "from" to visited list
        // get adyacents and add them to looking list in order os F value. On tie use H value
        // check if destiny is between them
        // get the minimum F and add it to visit list (remove from looking list)
        // repeat number two.
    return road;
}

Compass::~Compass() {}
