//
// Created by rodian on 18/05/17.
//

#include "unit.h"

Unit::Unit(int life, std::string type, int unit_speed, Size size, Size range,
           Map& map) : Occupant(life, type, size), compass(map,size),
       unit_speed(unit_speed), state("std"), team("neutral"), range(range) {}

void Unit::moveToPosition(int x, int y) {
    this->state = "mv";
    Position destination(x,y);
    std::vector<Position>* road = compass.getFastestWay(occ_size.getPosition(),
                                                        destination);

    // different quantity of positions by TIC depending on unit_speed and
    // terrain_factor
    bool arrived = false;
    while (this->state == "mv" && !arrived){
        //
        //
        Position pos = road->back();
        this->occ_size.moveTo(pos.getX(),pos.getY());
        road->pop_back();
        if (road->empty())
            arrived = true;
            //use of Tics to determine velocity
    }

    if (arrived)
        this->state = "std";
}

std::string Unit::getState() const {
    return this->state;
}

Position Unit::getCurrentPosition() const {
    return this->occ_size.getPosition();
}

void Unit::setTeam(std::string team) {
    this->team = team;
}
