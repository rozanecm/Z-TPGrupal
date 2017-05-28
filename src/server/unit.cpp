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
    int distance = unit_speed;
    int steps = 0;
    while (this->state == "mv" && !road->empty() && steps <= distance){
        Position pos = road->back();
        Size next_pos(pos.getX(),pos.getY(),occ_size.getWidth()
                                                        ,occ_size.getHeight());
        if (map.canIWalkToThisPosition(next_pos)) {
            double t_factor = map.getTerrainFactorOn(pos.getX(),pos.getY());
            this->occ_size.moveTo(pos.getX(),pos.getY());
            road->pop_back();

            // increase or decrease distance til steps are more than unit speed
            if (steps <= unit_speed && unit_speed == 4) {
                distance = (int) (t_factor * distance);
            } else if (steps <= unit_speed && unit_speed > 4) {
                distance = (int) (t_factor *distance *
                                                (1-(damage_recv/life_points)));
            }
            ++steps;
        } else {
            Position destiny = road->front();
            this->calculateRoadTo(destiny.getX(),destiny.getY());
            this->move();
        }
    }

    if (road->empty())
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


