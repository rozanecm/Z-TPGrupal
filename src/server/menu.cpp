//
// Created by rodian on 29/05/17.
//

#include "menu.h"

Menu::Menu(std::string& config) : lobby_counter(0), config(config) {}

void Menu::addPlayer(Messenger *msgr, Menu& menu, std::string player_id) {
    Lock l(m);
//    std::string player_id = msgr->recieveMessage();
    this->players.push_back(new Player(msgr, menu, player_id));
    this->players.back()->start();
}

void Menu::createNewLobby(Player* player) {
    lobby_counter += 1;

    Lobby* new_lobby = new Lobby(lobby_counter, config);
    std::cout << "Se creo el lobby"<< std::endl;
    lobbies.emplace_back(new_lobby);
    lobbies.back()->addPlayer(player);
    player->addLobby(new_lobby);
    player->getMessenger()->sendMessage("New Lobby created");
}

std::string Menu::getLobbiesInfo() {
    std::string info = "lobbyinfo-";
    for (int i = 0; i < lobbies.size(); ++i) {
        info += std::to_string(i) + "-";
    }
    return info;
}

void Menu::addToLobby(int id_lobby, Player* player) {
    if(lobbies.size() < id_lobby) {
        return;
    }
    lobbies[id_lobby]->addPlayer(player);
    player->addLobby(lobbies[id_lobby]);
}

Menu::~Menu() {
    for(auto p: players) {
        p->shutDown();
        p->join();
        delete(p);
    }

    for(auto l: lobbies) {
        delete(l);
    }
}
