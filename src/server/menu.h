//
// Created by rodian on 29/05/17.
//

#ifndef Z_TPGRUPAL_MENU_H
#define Z_TPGRUPAL_MENU_H


#include <vector>
#include "player.h"

class Menu {
private:
    std::vector<Player> players;
    std::mutex& m;
    // vector of lobbies
public:
    Menu(std::mutex& m);

    void addPlayer(Messenger* msgr, Menu& menu);

    void createNewLobbie(Player& player);

    std::string getLobbiesInfo();

    void addToLobbie(int id_lobbie, Player& player);
};


#endif //Z_TPGRUPAL_MENU_H
