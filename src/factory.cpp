//
// Created by rodian on 22/05/17.
//

#include "factory.h"

Factory::Factory(int life, std::string type, Size position) :
        Occupant(life,type, position), running(false), player("neutral") {}

std::string Factory::getPlayer() {
    return this->player;
}
