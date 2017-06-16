//
// Created by rodian on 10/06/17.
//

#include "territory.h"

Territory::Territory(std::map<int, Factory>& factories,
                     Position flag, Size size) :
Teamable("Neutral",territory_size),factories(factories) , flag_position(flag),
territory_size(size), changed(false){}

void Territory::grabFlag(std::string& new_team) {
    changed = true;
    this->changeTeam(new_team);
    for (auto fac: factories) {
        fac.second.changeTeam(new_team);
    }
}

int Territory::getTechLevel() {
    return (int)factories.size();
}

void Territory::changeFactoriesTechLevel(int tech_level) {
    for (auto fac: factories) {
        fac.second.changeTechLevel(tech_level);
    }
}

Position Territory::getFlagPosition() {
    return flag_position;
}

void Territory::build() {
    for (auto f: factories) {
        f.second.build();
    }
}

std::map<int, Factory> &Territory::getFactories() {
    return factories;
}

bool Territory::doesTerritorysOwnerChanged() {
    bool tmp = changed;
    changed = false;
    return tmp;
}


