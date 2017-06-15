//
// Created by rodian on 29/05/17.
//

#include "game.h"

// get terrain map from map loader
Game::Game(std::vector<Player *> players, std::vector<Messenger *> msgr,
           std::shared_ptr<Map> map, std::map<int, Unit*> units,
           std::vector<Team> teams_info, std::vector<Occupant*> occupants) :
           commands(m), players(players), all_units(units),
           control(msgr, all_units, occupants, teams_info,commands),
           map(map){}


//Game::Game(std::vector<Player*> players, std::shared_ptr<Map> map) : players(players),
//map(map){}

void Game::run() {
    std::string& map_str = map.get()->get_map();
    for(auto& player : players) {
        std::cout << "Sending map to players" << std::endl;
        std::string msg = "loadmap-" + map_str;
        player->getMessenger()->sendMessage(msg);
        player->addControlUnit(&control, &commands);
        std::cout << "size: " << map_str.size()<< std::endl;
    }
    control.run();
}

void Game::addBuildings(std::vector<Occupant> buildings) {

}

void Game::shutDownGame() {
    /*control unit shut down */
}


