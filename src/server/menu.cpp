//
// Created by rodian on 29/05/17.
//

#include "menu.h"

Menu::Menu(std::mutex& m) : m(m) {}

void Menu::addPlayer(Messenger *msgr, Menu& menu) {
    Lock l(m);
    std::string player_id = msgr->recieveMessage();
    this->players.push_back(new Player(player_id,msgr));
    this->players.back()->run();
}

void Menu::createNewLobbie(Player* player) {

}

std::string Menu::getLobbiesInfo() {
    std::string info = "No lobbies";
    // if (there are lobbies)
    // ....
    return info;
}

void Menu::addToLobbie(int id_lobbie, Player &player) {

}
