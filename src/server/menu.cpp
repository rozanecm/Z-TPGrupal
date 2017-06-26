//
// Created by rodian on 29/05/17.
//

#include <sstream>
#include <string>
#include "menu.h"

#define ERROR_MSG "joinlobby-error"
#define OK_MSG "joinlobby-ok"

Menu::Menu(std::string& config) : lobby_counter(0), config(config) {}

bool Menu::addPlayer(Messenger *msgr, Menu& menu, std::string player_id) {
    Lock l(m);
    for(Player* p : players) {
        if (p->getId() == player_id) {
            return false;
        }
    }
    this->players.push_back(new Player(msgr, menu, player_id));
    this->players.back()->start();
    return true;
}

void Menu::createNewLobby(Player* player) {
    Lock l(m);
    Lobby* new_lobby = new Lobby(lobby_counter++, config);
    std::cout << "Se creo el lobby"<< std::endl;
    lobbies.emplace_back(new_lobby);
    lobbies.back()->addPlayer(player);
    player->addLobby(new_lobby);
    player->getMessenger()->sendMessage(OK_MSG);
}

std::string Menu::getLobbiesInfo() {
    Lock l(m);
    std::string info = "lobbyinfo-";
    for (int i = 0; i < lobbies.size(); ++i) {
        info += std::to_string(i) + "-";
    }
    return info;
}

void Menu::addToLobby(int id_lobby, Player* player) {
    Lock l(m);
    for (Lobby* lobby : lobbies) {
        if (lobby->get_id() == id_lobby) {
            if (lobby->addPlayer(player)) {
                player->addLobby(lobby);
                player->getMessenger()->sendMessage(OK_MSG);
            } else {
                player->getMessenger()->sendMessage(ERROR_MSG);
            }
            return;

        }
    }
    player->getMessenger()->sendMessage(ERROR_MSG);
}

void Menu::shutDown() {
    for(auto p: players) {
        p->shutDown();
        p->join();
        delete(p);
    }

    for(auto l: lobbies) {
        l->shutDown();
        delete(l);
    }
}

Menu::~Menu() {}

std::string Menu::changeName(std::string &new_name) {
    Lock l(m);
    for(Player* p : players) {
        if (p->getId() == new_name) {
            return "error";
        }
    }
    return "ok";
}

void Menu::disconectPlayer(Player *player) {
    Lock l(m);
    std::vector<Player*>::iterator it = players.begin();
    for(;it != players.end();++it) {
        if ((*it)->getId() == player->getId()) {
            (*it)->shutDown();
            (*it)->join();
            delete((*it));
            players.erase(it);
            break;
        }
    }
}

