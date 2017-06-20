//
// Created by rodian on 29/05/17.
//

#include "player.h"


Player::Player(Messenger *msg, Menu &menu, std::string& id) :
        messenger(msg),id(id), conected(true),on_menu(true),on_lobby(false),
        playing(false),menu(menu),ready(false) {}

void Player::run() {
    try {
        while (messenger->isConnected()) {
            std::string new_cmd = messenger->recieveMessage();

            std::cout << "Player " << id << " ejecuta " << new_cmd << std::endl;
            if (on_menu) {
                processMenuCommands(new_cmd);
            } else if (on_lobby) {
                processLobbyCommands(new_cmd);
            } else if (playing) {
                commands->addCommand(this->id, new_cmd, control);
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

void Player::addControlUnit(ControlUnit *control, CommandMonitor* commands) {
    this->playing = true;
    this->control = control;
    this->commands = commands;
}

Messenger *Player::getMessenger() {
    return messenger;
}

void Player::addLobby(Lobby* lobby) {
    this->lobby = lobby;
    on_lobby = true;
}

void Player::processMenuCommands(std::string &full_cmd) {
    std::string cmd = getNextData(full_cmd);
    if (cmd == "createlobby") {
        this->menu.createNewLobby(this);
        on_menu = false;
    }  else if (cmd == "getinlobby") {
        std::string lobby_id = getNextData(full_cmd);
        int id = std::stoi(lobby_id);
        this->menu.addToLobby(id,this);
        on_menu = false;
    }  else if (cmd == "lobbyinfo") {
        messenger->sendMessage(this->menu.getLobbiesInfo());
    } else {
        messenger->sendMessage("Invalid cmd");
    }
}

void Player::processLobbyCommands(std::string &cmd) {
    if (cmd == "startgame") {
        std::cout << "Entre en player a start game" << std::endl;
        this->lobby->startGame();
    } else if (cmd == "ready") {
        this->ready = true;
        this->lobby->ready(this);
    }  else {
        messenger->sendMessage("Invalid cmd");
    }
}

Player::~Player() {}

void Player::shutDown() {
    conected = false;
    messenger->shutdown();
}

std::string Player::getId() const {
    return id;
}

std::string Player::getNextData(std::string& line) {
    std::size_t found = line.find('-');
    std::string data = line.substr(0,found);
    line.erase(0,found+1);
    return data;
}

void Player::getInGame() {
    messenger->sendMessage("startgame");
    this->on_lobby = false;
    this->playing = true;
}

bool Player::areYouReady() {
    return this->ready;
}

