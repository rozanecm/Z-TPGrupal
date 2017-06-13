//
// Created by rodian on 29/05/17.
//

#include "lobby.h"
#define PATH "maps/map.xml"

Lobby::Lobby(int id, std::string& config) : lobby_id(id),
                                            config(config),
                                            all_ready(false) {}

void Lobby::startGame() {
    std::cout << "Beginning game" << std::endl;
    if(all_ready){
        //start game
        MapLoader maploader(PATH, config);
        std::shared_ptr<Map> map = maploader.get_map();
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
        teams[players.size()][0] = player->getId();
    return (players.size() < 4);
}

Lobby::~Lobby() {
    game.get()->shutDownGame();
    game.get()->join();
}
