//
// Created by rodian on 29/05/17.
//

#include "player.h"

Player::Player(Messenger *msg, ControlUnit* control, Menu& menu) :
        messenger(msg), conected(true),on_menu(true),on_lobby(false),
        playing(false), control(control), menu(menu) {

}

Player::Player(Messenger *msg, Menu& menu) :
        messenger(msg), conected(true),on_menu(true),on_lobby(false),
        playing(false),menu(menu) {}

void Player::run() {
    try {
        while (messenger->isConnected()) {
            std::string new_cmd = messenger->recieveMessage();

            std::cout << "Player " << id << "ejecuta " << new_cmd << std::endl;
            if (on_menu) {
                processMenuCommands(new_cmd);
            } else if (on_lobby) {
                processLobbyCommands(new_cmd);
            } else if (playing) {
                cmds.push_back(new Command(this->id, new_cmd, control));
            }
        }
    } catch(SocketError e) {
        conected = false;
        std::cout << "Player " << id << " desconectado." << std::endl;
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

void Player::addLobby(Lobby* lobby) {
    this->lobby = lobby;
    on_lobby = true;
}

void Player::processMenuCommands(std::string &cmd) {
    if (cmd == "create-lobby") {
        this->menu.createNewLobby(this);
        on_menu = false;
    } else {
        messenger->sendMessage("Invalid cmd");
    }
}

void Player::processLobbyCommands(std::string &cmd) {
    if (cmd == "start-game") {
        std::cout << "Entre en player a start game" << std::endl;
        this->lobby->startGame();
        on_lobby = false;
    }
    if (cmd == "ready") {
        this->lobby->ready(this);
    } else {
        messenger->sendMessage("Invalid cmd");
    }
}

Player::~Player() {}

void Player::shutDown() {
    conected = false;
    messenger->shutdown();
}

