//
// Created by rodian on 18/05/17.
//

#include "unit.h"

Unit::Unit(int life, int id, Size size, Map& map) :
                 Occupant(life, id, size/*, map*/), compass(map,size) {}

void Unit::setNewPosition(int x, int y) {
    this->occ_size.moveTo(x,y);
}

