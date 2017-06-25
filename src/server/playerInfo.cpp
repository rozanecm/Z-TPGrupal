//
// Created by rodian on 12/06/17.
//

#include "playerInfo.h"

PlayerInfo::PlayerInfo(std::string id, Factory *fortress) :
        id(id), fortress(fortress), tech_level(1) {}


PlayerInfo::PlayerInfo(std::string id) : id(id), tech_level(0) {}

std::string PlayerInfo::getPlayerId() const {
    return id;
}

bool PlayerInfo::checkIfFortressLives() {
    return fortress->areYouAlive();
}

int PlayerInfo::getTechLevel() {
    return tech_level;
}

void PlayerInfo::increaseTechLevel() {
    this->tech_level += 1;
}

void PlayerInfo::decreaseTechLevel() {
    this->tech_level -= 1;
}

Factory *PlayerInfo::getFortress() {
    return fortress;
}

void PlayerInfo::addMessenger(Messenger *messenger) {
    this->player_messenger = messenger;
}

Messenger *PlayerInfo::getMessenger() {
    return this->player_messenger;
}

void PlayerInfo::addTerritory(Territory *territory) {
    territories.push_back(territory);
    recalculateTechLevel();
}

void PlayerInfo::recalculateTechLevel() {
    tech_level = 0;
    for (auto& t: territories) {
        tech_level += t->getTechLevel();
    }

    for (auto& t: territories) {
        t->changeFactoriesTechLevel(tech_level);
    }
}

void PlayerInfo::eliminateThisTerritory(Territory* territory) {
    std::vector<Territory*>::iterator it = territories.begin();
    for (; it != territories.end();) {
        if (territory->getId() == (*it)->getId()) {
            it = territories.erase(it);
        } else {
            ++it;
        }
    }
    recalculateTechLevel();
}

PlayerInfo::~PlayerInfo() {
    player_messenger = nullptr;
}

bool PlayerInfo::areYouStillConected() {
    return player_messenger->isConnected();
}

void PlayerInfo::addFortress(Factory *fortress) {
    this->fortress = fortress;
}

