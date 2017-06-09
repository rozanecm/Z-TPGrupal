//
// Created by rodian on 29/05/17.
//

#include "menu.h"

Menu::Menu(std::mutex& m) : m(m),lobby_counter(0) {}

void Menu::addPlayer(Messenger *msgr, Menu& menu) {
    Lock l(m);
//    std::string player_id = msgr->recieveMessage();
    this->players.push_back(new Player(msgr,menu));
    this->players.back()->start();
    std::cout << "new player en menu" << std::endl;
}

void Menu::createNewLobby(Player* player) {
    std::cout << "Creando Lobby" << std::endl;
    lobby_counter += 1;
    Lobby* new_lobby = new Lobby(lobby_counter);
    lobbys.emplace_back(new_lobby);
    lobbys.back()->addPlayer(player);
    player->addLobby(new_lobby);
    player->getMessenger()->sendMessage("New Lobby created");
}

std::string Menu::getLobbiesInfo() {
    std::string info = "No lobbies";
    // if (there are lobbies)
    // ....
    return info;
}

void Menu::addToLobbie(int id_lobbie, Player &player) {

}

Menu::~Menu() {
    for(auto p: players) {
        p->join();
        delete(p);
    }
}
