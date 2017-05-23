//
// Created by rodian on 18/05/17.
//

#include "unit.h"

Unit::Unit(int life, std::string type, int unit_speed, Size size, Size range,
      Map& map) : Occupant(life, type, size), Teamable(size),
compass(map,size,unit_speed), unit_speed(unit_speed), state("std"),
range(range), map(map) {}

void Unit::calculateRoadTo(int x, int y) {
    this->state = "mv";
    Position destination(x,y);
    road = compass.getFastestWay(occ_size.getPosition(),destination);
}

void Unit::move() {
    // different quantity of positions by TIC depending on unit_speed and
    // terrain_factor
    bool arrived = false;
    while (this->state == "mv" && !road->empty()){
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


void Unit::grab(Teamable* object, std::string u_type) {
    // move to position
    if (u_type == "Flag") {
        object->changeTeam(this->team);
    } else if (this->type == "Grunt") { // Only Grunt robots can drive
        // If is not a flag, is a vehicle
        // Unit disappears right before taking the vehicle
        object->changeTeam(this->team);
        this->life_points = 0;
    }
}


