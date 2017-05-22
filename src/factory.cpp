//
// Created by rodian on 22/05/17.
//

#include "factory.h"

Factory::Factory(int life, std::string type, Size position) :
        Occupant(life,type, position), Teamable(position), running(false) {}

Factory::Factory(int life, std::string type, std::string team, Size position) :
    Occupant(life,type, position), Teamable(team,position), running(false) {}


void Factory::setNewPlayer(std::string player, int tech_level) {
    this->tech_level = tech_level;
    this->team = player;
}


