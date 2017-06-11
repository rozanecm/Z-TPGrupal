//
// Created by rodian on 22/05/17.
//

#include "factory.h"
#define ID 3
Factory::Factory(int id, int life, std::string type, Size position,
                 std::vector<UnitMold> units, Map &map,
                 std::map<std::string, Weapon> &weapons) :
Occupant(id, life,type, position), running(false),time_counter(0), units(units),
map(map), weapons(weapons){
    it = units.begin();
}

void Factory::build() {
    if (time_counter == (*it).getCreationTime()) {
        Size u_size = (*it).getUnitSize();
        Position factory_pos = this->obj_size.getPosition();
        u_size.moveTo(factory_pos.getX(),factory_pos.getY());
        (*it).createUnit(ID,u_size,map,weapons[(*it).getTypeOfUnit()]);
        time_counter = 0;
    } else if (running && time_counter < (*it).getCreationTime()) {
        time_counter += 1 + tech_level;
    }
}

void Factory::stopBuilding(std::string &player_id) {
    if (player_id == this->getTeam())
        this->running = false;
}

int Factory::getSelectedUnitTime() {
    return it->getCreationTime();
}

std::string Factory::nextUnit() {
    int i = 0;
    while (i == 0 || it->getTechnologyLevel() > this->tech_level) {
        ++it;
        if (it == units.end())
            it = units.begin();
        ++i;
    }
    return it->getTypeOfUnit();
}

Occupant *Factory::destroyFactory() {
    // capaz necesite un tipo distinto a Occupant para que no pueda ser
    // destruido ni atacado
//    Occupant* ruins = new Occupant(,1000,"Ruins",this->occ_size);
//    return ruins;
}

void Factory::changeTechLevel(int tech_level) {
    this->tech_level = tech_level;
}

void Factory::startBuilding(std::string &player_id) {
    if (player_id == this->getTeam())
        running = true;
}

int Factory::getCreationSpeed() {
    return ((*it).getCreationTime() / (1 + tech_level));
}

bool Factory::haveNewUnits() {
    return (!this->new_units.empty());
}

std::vector<Unit> Factory::getUnits() {
    std::vector<Unit> tmp = new_units;
    new_units.clear();
    return tmp;
}
