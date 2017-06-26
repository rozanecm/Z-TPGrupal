//
// Created by rodian on 10/06/17.
//

#include "territory.h"
#define FLAGWIDTH 2
#define FLAGHEIGHT 3
Territory::Territory(const std::map<int, Factory *> &factories, Position flag,
                     Size size, int id) :
Teamable("Neutral",territory_size),factories(factories) ,
flag("Neutral",Size(flag.getX(),flag.getY(),FLAGWIDTH,FLAGHEIGHT)),
territory_size(size), id(id){}

void Territory::grabFlag(std::string& new_team) {
    changed = true;
    this->changeTeam(new_team);
    for (auto fac: factories) {
        fac.second->changeTeam(new_team);
    }
}

int Territory::getTechLevel() {
    return (int)factories.size();
}

void Territory::changeFactoriesTechLevel(int tech_level) {
    for (auto fac: factories) {
        fac.second->changeTechLevel(tech_level);
    }
}

std::map<int, Factory*> &Territory::getFactories() {
    return factories;
}

bool Territory::doesTerritorysOwnerChanged() {
    if (this->flag.getTeam() != this->team) {
        std::string new_team = flag.getTeam();
        grabFlag(new_team);
    }
    bool tmp = changed;
    changed = false;
    return tmp;
}

int Territory::getId() {
    return id;
}

Teamable *Territory::getFlag() {
    return &flag;
}

Territory::~Territory() {
    for(auto& f: factories) {
        delete(f.second);
    }
    factories.clear();
}


