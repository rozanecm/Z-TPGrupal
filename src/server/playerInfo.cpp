//
// Created by rodian on 12/06/17.
//

#include "playerInfo.h"

PlayerInfo::PlayerInfo(std::string id, Factory *fortress) :
        id(id), fortress(fortress), tech_level(1) {}

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
