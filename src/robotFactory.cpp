//
// Created by rodian on 22/05/17.
//

#include "robotFactory.h"

RobotFactory::RobotFactory(int life, std::string type, Size position,
                           std::vector<UnitMold> units) :
        Factory(life,type, position), units(units){
    it = units.begin();
}

void RobotFactory::build() {
    this->running = true;

    while (running) {
    // after sleeping time expected
    // create the correct amount of units
    }
}

void RobotFactory::stopBuilding() {
    this->running = false;
}

int RobotFactory::getSelectedUnitTime() {
    return it->getCreationTime();
}

std::string RobotFactory::nextUnit() {
    if ( ++it == units.end())
        it = units.begin();
    return it->getTypeOfUnit();
}

Occupant *RobotFactory::destroyFactory() {
    // capaz necesite un tipo distinto a Occupant para que no pueda ser
    // destruido ni atacado
    Occupant* ruins = new Occupant(1000,"Ruins",this->occ_size);
    return ruins;
}
