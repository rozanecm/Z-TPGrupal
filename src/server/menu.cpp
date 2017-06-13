//
// Created by rodian on 29/05/17.
//

#include "menu.h"

Menu::Menu(std::string& config) : lobby_counter(0), config(config) {}

void Menu::addPlayer(Messenger *msgr, Menu& menu) {
    Lock l(m);
    this->players.push_back(new Player(msgr,menu));
    this->players.back()->start();
}

void Menu::createNewLobby(Player* player) {
    lobby_counter += 1;
    Lobby* new_lobby = new Lobby(lobby_counter, config);
    lobbies.emplace_back(new_lobby);
    lobbies.back()->addPlayer(player);
    player->addLobby(new_lobby);
    player->getMessenger()->sendMessage("New Lobby created");
}

std::string Menu::getLobbiesInfo() {
    std::string info = "No lobbies";
    // if (there are lobbies)
    // ....
    return info;
}

void Menu::addToLobby(int id_lobbie, Player &player) {

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
