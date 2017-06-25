//
// Created by rodian on 29/05/17.
//

#include "game.h"
#define FORTRESS "Fort"
#define VFABRIC "vehiculeFabric"
#define RFABRIC "robotFabric"

// get terrain map from map loader
//Game::Game(std::vector<Player *> players, std::vector<Messenger *> msgr,
//           std::shared_ptr<Map> map, std::map<int, Unit *> units,
//           std::vector<Team>& teams_info, std::vector<Occupant *> occupants,
//           std::vector<Territory *> &terr) :
//           commands(m), players(players),all_occupants(occupants),
//           territories(terr), teams(teams_info),all_units(units),
//           control(msgr, all_units, all_occupants, teams,
//                   commands, territories),
//           map(map){}


Game::Game(std::string path, std::string &config, std::vector<Team> &teams_info,
           std::vector<Player *> &players) :
        commands(m),teams(teams_info), path(path), config(config),
        players(players) {}

void Game::run() {
    this->buildMap();
    this->sincronizeOccupants();
    std::vector<Messenger*> messengers = getMessengers();
    ControlUnit control(messengers,all_units,all_occupants,
                        teams,commands,territories);
    this->sendMapInfo(control);
    this->buildTypeMap();
    this->sendTerritoryInfo();
    this->sendOccupantsInfo();
    control.run();
}

void Game::buildMap() {
    MapLoader maploader(path,config);
    map = maploader.get_map();

    all_occupants = map->getOccupants();

    // add a Fortress to each player
    std::vector<Factory*> forts = maploader.get_forts();
    for (auto& t: teams) {
        std::vector<PlayerInfo>& playersInfo = t.getPlayersInfo();
        for (auto& p: playersInfo) {
            Factory* fortress = forts.back();
            fortress->changeTeam(p.getPlayerId());
            // set changed boolean to false
            fortress->haveYouChanged();
            p.addFortress(fortress);
            all_occupants.push_back((Occupant*) fortress);
            forts.pop_back();
        }
    }

    territories = maploader.get_territories();
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

void Game::sendMapInfo(ControlUnit &control) {
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
        for (auto& team: teams) {
            std::vector<PlayerInfo>& players = team.getPlayersInfo();
            for (auto& p : players) {
                Factory* fortress = p.getFortress();
                for (auto& f: factories) {
                    if (fortress->getId() == f.second->getId()) {
                        std::string new_team = p.getPlayerId();
                        t->grabFlag(new_team);
                        p.addTerritory(t);
                    }
                }
            }
        }
    }
    map->updateOccupants(&all_occupants);
    this->createStartingUnits();
}

void Game::sendTerritoryInfo() {
    std::string info = "";
    for (auto& t: territories) {
        info = "updateterritory-";
        info += std::to_string(t->getId()) + "-"  + t->getTeam() + "-";
        Position flag = t->getFlag()->getPosition();
        info += std::to_string(flag.getX()) + "-" +
                std::to_string(flag.getY())+ "|";
    }
    for(auto& player : players) {
        player->getMessenger()->sendMessage(info);
    }
}

void Game::createStartingUnits() {
    for (auto& team: teams) {
        std::vector<PlayerInfo>& players = team.getPlayersInfo();
        for (auto& p : players) {
            Factory* fortress = p.getFortress();
            int id_counter = (int)(territories.size() + all_occupants.size());
            fortress->createStartingUnits(id_counter);
            std::vector<Unit *> tmp = fortress->getUnits();
            for (auto &u: tmp) {
                u->recalculateMyStartPosition();
                all_units[u->getId()] = u;
                all_occupants.push_back((Occupant*)u);
                // set changed boolean to false
                u->haveYouChanged();
            }
        }
    }
}

std::vector<Messenger *> Game::getMessengers() {
    std::vector<Messenger *> messengers;
    for (auto& p: players) {
        messengers.push_back(p->getMessenger());
    }
    return messengers;
}


