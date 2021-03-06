//
// Created by rodian on 22/05/17.
//

#include <memory>
#include "factory.h"
#define ID 3
Factory::Factory(int id, int life, std::string& type, Size position,
                 std::vector<UnitMold*>& units, std::shared_ptr<Map> map,
                 std::map<std::string, Weapon> &weapons) :
Occupant(id, life,type, position), running(false),time_counter(0), units(units),
map(map), weapons(weapons), tech_level(1){
    mold = units.begin();
}

void Factory::build(int& id_counter) {
    if (time_counter >= (*mold)->getCreationTime()) {
        Size u_size = (*mold)->getUnitSize();
        Position factory_pos = this->obj_size.getPosition();
        u_size.moveTo(factory_pos.getX(),factory_pos.getY());
        Weapon u_weapon = weapons.at((*mold)->getWeaponType());

        for (int i = 0; i < (*mold)->getCreationQuantity(); ++i) {
            Unit* new_unit = (*mold)->createUnit(
                    id_counter,u_size,*map,u_weapon);
            new_unit->changeTeam(this->team);
            new_units.push_back(new_unit);
            ++id_counter;
        }
        this->changed = true;
        time_counter = 0;
    } else if (running && time_counter < (*mold)->getCreationTime()) {
        time_counter += 1 + tech_level;
        this->changed = true;
    } else {
        this->changed = false;
    }
}

int Factory::getSelectedUnitTime() {
    return (*mold)->getCreationTime();
}

UnitMold* Factory::nextUnit() {
    int i = 0;
    this->running = false;
    time_counter = 0;
    while (i == 0 || (*mold)->getTechnologyLevel() > this->tech_level) {
        ++mold;
        if (mold == units.end())
            mold = units.begin();
        ++i;
    }
    return *mold;
}

void Factory::changeTechLevel(int tech_level) {
    this->tech_level = tech_level;
}

void Factory::startBuilding(const std::string &player_id) {
    if (player_id == this->getTeam())
        running = true;
}

int Factory::getCreationSpeed() {
    return (((*mold)->getCreationTime() - time_counter) / (1 + tech_level));
}

bool Factory::haveNewUnits() {
    return (!this->new_units.empty());
}

std::vector<Unit*> Factory::getUnits() {
    std::vector<Unit*> tmp = new_units;
    new_units.clear();
    return tmp;
}

UnitMold * Factory::getSelectedUnit() {
    return *mold;
}

UnitMold* Factory::previousUnit() {
    int i = 0;
    this->running = false;
    time_counter = 0;
    while (i == 0 || (*mold)->getTechnologyLevel() > this->tech_level) {
        if (mold == units.begin()) {
            mold = units.end();
        }
        --mold;
        ++i;
    }
    return *mold;
}

void Factory::resetSelectedUnit() {
    mold = units.begin();
}

void Factory::createStartingUnits(int &id_counter) {
    time_counter = (*mold)->getCreationTime();
    this->build(id_counter);
}

Factory::~Factory() {}

std::vector<UnitMold *> Factory::getMolds() {
    return units;
}

