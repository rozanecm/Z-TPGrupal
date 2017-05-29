//
// Created by rodian on 29/05/17.
//

#include "lobby.h"

Lobby::Lobby(int id) : lobby_id(id), all_ready(false) {}

void Lobby::startGame() {
    if(all_ready){
        //start game
    }
}

void Lobby::ready() {
    all_ready = true;
}

bool Lobby::addPlayer(Player* player) {
    if(players.size() < 4)
        players.push_back(player);
    return (players.size() < 4);
}
