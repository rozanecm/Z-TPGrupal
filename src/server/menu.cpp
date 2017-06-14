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
    std::cout << "lo pongo en el vector"<< std::endl;
    lobbies.back()->addPlayer(player);
    std::cout << "agrego player"<< std::endl;
    player->addLobby(new_lobby);
    std::cout << "agregoel lobby en el player"<< std::endl;
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
