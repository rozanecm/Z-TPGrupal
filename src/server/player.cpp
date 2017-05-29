//
// Created by rodian on 29/05/17.
//

#include "player.h"

Player::Player(std::string id, Messenger *msg, Menu &menu) :
        id(id), messenger(msg), conected(true), playing(false), menu(menu) {}

void Player::run() {
    while (conected) {
        std::string new_cmd = messenger->recieveMessage();
        cmds->emplace_back(new_cmd);
    }
}

