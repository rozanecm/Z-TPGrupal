//
// Created by rodian on 29/05/17.
//

#include "game.h"

// get terrain map from map loader
Game::Game(std::vector<Messenger*> players, Map& map,
           std::vector<Unit> units) : players(players),
           control(players,units,map.getOccupants()),
           map(map){}

void Game::run() {
    std::string& map_str = map.get_map();
    for(auto& player : players) {
        std::cout << "Sending map to players" << std::endl;
        player->sendMessage(map_str);
    }
    control.run();
}

void Game::addBuildings(std::vector<Occupant> buildings) {

}

