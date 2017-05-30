//
// Created by rodian on 29/05/17.
//

#include "player.h"

Player::Player(std::string id, Messenger *msg, ControlUnit* control, Menu& menu) :
        id(id), messenger(msg), conected(true), playing(false), control(control)
        ,menu(menu) {}

Player::Player(std::string id, Messenger *msg, Menu& menu) :
        id(id), messenger(msg), conected(true), playing(false),menu(menu) {}

void Player::run() {
    while (conected) {
        std::string new_cmd = messenger->recieveMessage();
        cmds.push_back(new Command(new_cmd,control));
    }
}

void Player::updateInfo(std::string &info) {
    messenger->sendMessage(info);
}

void Player::addControlUnit(ControlUnit *control) {
    this->control = control;
}

Messenger *Player::getMessenger() {
    return messenger;
}

