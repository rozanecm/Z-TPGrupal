//
// Created by rodian on 18/05/17.
//

#include "unit.h"

Unit::Unit(int id, int life, std::string type, int unit_speed, Size size,
          Size range, Compass& compass) : Occupant(id,life, type, size),
          compass(compass), unit_speed(unit_speed), state(STANDINGSTATE),
            range(range), target(*this) {}

void Unit::makeAction() {
    if (this->state == STANDINGSTATE) {
        // Check for enemies around you. If so, state = ATKSTATE
        //if (enemiesOnRange())
//            this->state = ATKSTATE;
    }
    if (this->state == MOVESTATE) {
        this->move();
    }
    if (this->state == ATKSTATE) {
        //attack();
    }
}

void Unit::calculateRoadTo(int x, int y) {
    this->state = MOVESTATE;
    Position destination(x,y);
    road = compass.getFastestWay(obj_size.getPosition(),destination);
}

void Unit::move() {
    int distance = unit_speed;
    int steps = 0;
    while (this->state == MOVESTATE && !road->empty() && steps <= distance){
        Position pos = road->back();
        Size next_pos(pos.getX(),pos.getY(),obj_size.getWidth()
                                                        ,obj_size.getHeight());
        if (compass.canIWalkToThisPosition(next_pos)) {
            double t_factor = compass.getTerrainFactorOn(pos.getX(),pos.getY());
            this->obj_size.moveTo(pos.getX(),pos.getY());
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
        this->state = STANDINGSTATE;
}

void Unit::attack() {
    //check if target it's on range
    Size trg_size = target.getSize();
    if (range.isThereACollision(trg_size)) {
        // make a shot
    } else {
        road = compass.getFastestWay(obj_size.getPosition(),
                                     target.getPosition());
        move();
    }
}

std::string Unit::getState() const {
    return this->state;
}

Position Unit::getCurrentPosition() const {
    return this->obj_size.getPosition();
}

void Unit::grab(Teamable* object, std::string u_type) {
    // move to position
    if (u_type == FLAGTYPE) {
        object->changeTeam(this->team);
    } else if (this->type == GRUNTTYPE) { // Only Grunt robots can drive
        // If is not a flag, is a vehicle
        // Unit disappears right before taking the vehicle
        object->changeTeam(this->team);
        this->life_points = 0;
    }
}

void Unit::setTargetToAttack(Occupant &target) {
    this->state == ATKSTATE;
    this->target = target;
}

std::vector<Bullet> Unit::collectBullets() {
    return std::vector<Bullet>();
}


Unit::Unit(const Unit &other) : compass(other.compass), unit_speed(other.unit_speed),
state(other.state), range(other.range), road(other.road), target(other.target){}

//Unit &Unit::operator=(const Unit &other) {
//    return ;
//}




