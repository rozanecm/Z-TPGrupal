//
// Created by rodian on 29/05/17.
//

#include "game.h"
#define FORTRESS "Fort"
#define VFABRIC "vehiculeFabric"
#define RFABRIC "robotFabric"

// get terrain map from map loader
Game::Game(std::vector<Player *> players, std::vector<Messenger *> msgr,
           std::shared_ptr<Map> map, std::map<int, Unit *> units,
           std::vector<Team>& teams_info, std::vector<Occupant *> occupants,
           std::vector<Territory *> &terr) :
           commands(m), players(players),all_occupants(occupants),
           territories(terr), teams(teams_info),all_units(units),
           control(msgr, all_units, all_occupants, teams,
                   commands, territories),
           map(map){}


//Game::Game(std::vector<Player*> players, std::shared_ptr<Map> map) : players(players),
//map(map){}

void Game::run() {
    sincronizeOccupants();
    map->updateOccupants(&all_occupants);
    this->sendMapInfo();
    this->buildTypeMap();
    this->sendOccupantsInfo();
    control.run();
}

void Game::addBuildings(std::vector<Occupant> buildings) {

}

void Game::shutDownGame() {
    /*control unit shut down */
}

void Game::sendOccupantsInfo() {
    std::string info;
    this->analyseOccupantsInfo(info);
    for(auto& player : players) {
        player->getMessenger()->sendMessage(info);
    }
}

void Game::sendMapInfo() {
    std::string& map_str = map.get()->get_map();
    for(auto& player : players) {
        std::cout << "Sending map to players" << std::endl;
        std::string msg = "loadmap-" + map_str;
        player->getMessenger()->sendMessage(msg);
        player->addControlUnit(&control, &commands);
        std::cout << "size: " << map_str.size()<< std::endl;
    }
}

void Game::analyseOccupantsInfo(std::string& info) {
    for (auto o: all_occupants) {
        if (types[o->getType()] == "Nature") {
            info += "addnature-";
        } else if (types[o->getType()] == "Building") {
            info += "addbuilding-";
        } else if (types[o->getType()] == "Unit") {
            info += "addunit-";
        }
        info += std::to_string(o->getId()) + "-";
        Position pos = o->getPosition();
        info += std::to_string(pos.getX()) + "-";
        info += std::to_string(pos.getY()) + "-";
        info += o->getType() + "-";
        info += o->getTeam() + "-";
        info += std::to_string(o->getLifeLeft()) + "|";  // life left == max hp
    }
}

void Game::buildTypeMap() {
    types.insert(std::pair<std::string,std::string>("Fort","Building"));
    types.insert(std::pair<std::string,std::string>
                         ("vehiculeFactory","Building"));
    types.insert(std::pair<std::string,std::string>("robotFactory","Building"));
    types.insert(std::pair<std::string,std::string>("Factory","Building"));
    types.insert(std::pair<std::string,std::string>("Rock","Nature"));
    types.insert(std::pair<std::string,std::string>("iceblock","Nature"));
    types.insert(std::pair<std::string,std::string>("grunt","Unit"));
    types.insert(std::pair<std::string,std::string>("Psycho","Unit"));
    types.insert(std::pair<std::string,std::string>("Tough","Unit"));
    types.insert(std::pair<std::string,std::string>("Pyro","Unit"));
    types.insert(std::pair<std::string,std::string>("Sniper","Unit"));
    types.insert(std::pair<std::string,std::string>("laser","Unit"));
    types.insert(std::pair<std::string,std::string>("jeep","Unit"));
    types.insert(std::pair<std::string,std::string>("MediumTank","Unit"));
    types.insert(std::pair<std::string,std::string>("LightTank","Unit"));
    types.insert(std::pair<std::string,std::string>("HeavyTank","Unit"));
    types.insert(std::pair<std::string,std::string>("MML","Unit"));
}

void Game::sincronizeOccupants() {
    for (auto& t: territories) {
        std::map<int, Factory*>& factories = t->getFactories();
        for (auto& f: factories) {
            (f.second)->resetSelectedUnit();
            all_occupants.push_back((Occupant*)(f.second));
        }
//        for (auto& team: teams) {
//            std::vector<PlayerInfo>& players = team.getPlayersInfo();
//            for (auto& p : players) {
//                Factory* fortress = p.getFortress();
//                all_occupants.push_back((Occupant*)fortress);
//            }
//        }
    }
}


