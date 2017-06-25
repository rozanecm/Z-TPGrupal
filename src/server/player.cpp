//
// Created by rodian on 29/05/17.
//

#include <sstream>
#include "player.h"

#define RETURNTOMENU "returntomenu"
#define ERROR "error"
#define OK "ok"

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
            } else if (new_cmd == RETURNTOMENU) {
                this->playing = false;
                this->on_menu = true;
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
    }  else if (cmd == "joinlobby") {
        std::string lobby_id = getNextData(full_cmd);
        int id = std::stoi(lobby_id);
        this->menu.addToLobby(id,this);
        on_menu = false;
    }  else if (cmd == "lobbyinfo") {
        messenger->sendMessage(this->menu.getLobbiesInfo());
    }   else if (cmd == "changename") {
        std::string new_name = getNextData(full_cmd);
        std::string ans = this->menu.changeName(new_name);
        if (ans == ("Successfully change name to " + new_name))
            this->id = new_name;
        messenger->sendMessage(ans);
    } else {
        messenger->sendMessage("Invalid cmd");
    }
}

void Player::processLobbyCommands(std::string &full_cmd) {
    std::string cmd = getNextData(full_cmd);
    if (cmd == "startgame") {
        std::string map = getNextData(full_cmd);
        this->lobby->startGame(map);
    } else if (cmd == "ready") {
        this->ready = true;
        this->lobby->ready(this);
    }  else if (cmd == "unready") {
        this->ready = false;
        this->lobby->unReady();
    }  else if (cmd == "exitlobby") {
        this->ready = false;
        this->lobby->unReady();
        this->on_lobby = false;
        this->on_menu = true;
        this->lobby->exitLobby(this);
    } else if (cmd == "mapsinfo") {
        messenger->sendMessage(lobby->get_loaded_maps());
    } else {
        messenger->sendMessage("Invalid cmd");
    }
}

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
    // Notify the client the game is starting
    std::stringstream msg;
    msg << "startgame-" << OK;
    messenger->sendMessage(msg.str());
    this->on_lobby = false;
    this->playing = true;
}

bool Player::areYouReady() {
    return this->ready;
}

Player::~Player() {
    commands = nullptr;
    control = nullptr;
    lobby = nullptr;
}
