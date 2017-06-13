//
// Created by rodian on 29/05/17.
//

#include "lobby.h"
#define PATH "maps/map.xml"

Lobby::Lobby(int id) : lobby_id(id), all_ready(false) {}

void Lobby::startGame() {
    std::cout << "Beginning game" << std::endl;
    if(all_ready){
        //start game
        MapLoader maploader(PATH);
        Map map = maploader.get_map();
        game = std::unique_ptr<Game> (new Game(players,map));
        game.get()->start();
        std::cout << "Game started" << std::endl;
    }
}

void Lobby::ready(Player* player) {
    std::cout << "all ready" << std::endl;
    all_ready = true;
    player->getMessenger()->sendMessage("ready-to-go");
}

bool Lobby::addPlayer(Player* player) {
    if(players.size() < 4)
        players.push_back(player);
    return (players.size() < 4);
}

Lobby::~Lobby() {
    game.get()->shutDownGame();
    game.get()->join();
}
