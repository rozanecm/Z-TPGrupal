//
// Created by rodian on 10/06/17.
//

#include "territory.h"

Territory::Territory(std::map<int, Factory>& factories, std::string team,
                     Position flag, Size size) :
Teamable(team,territory_size),factories(factories) , flag_position(flag),
territory_size(size){}

void Territory::grabFlag(std::string& new_team) {
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

Position Territory::getFlagPOsition() {
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


