//
// Created by rodian on 29/05/17.
//

#include <sstream>
#include "lobby.h"

Lobby::Lobby(int id, std::string& config) : lobby_id(id),
                                            config(config),
                                            all_ready(false),
                                            game_started(false){
    load_maps();
}

bool Lobby::startGame(const std::string& map_name) {
    std::cout << "Beginning game" << std::endl;
    if(all_ready){
        //start game
        game_started = true;
        auto path_it = maps.find(map_name);
        if (path_it == maps.end()) {
            return false;
        }
        std::string path = path_it->second;
        MapLoader maploader(path, config);
        std::shared_ptr<Map> map = maploader.get_map();

        std::vector<Occupant*>& occupants = map->getOccupants();
        std::map<int, Unit*> units;

        // build teams
        std::vector<Team> teams_info;
        for (int i = 0; i < teams.size(); ++i) {
            std::vector<PlayerInfo> playersInfo;
            for (int j = 0; j < teams[i].size(); ++j) {
                PlayerInfo new_player(teams[i][j]);
                for (auto p: players) {
                    if (p->getId() == teams[i][j])
                        new_player.addMessenger(p->getMessenger());
                }
                playersInfo.push_back(new_player);
            }
            Team new_team(playersInfo,i);
            teams_info.push_back(new_team);
        }

        // get messengers
//        std::vector<Messenger *> messengers;
        for (auto p: players) {
            p->getInGame();
        }

        std::vector<Territory *> territories = maploader.get_territories();
//        game = std::unique_ptr<Game> (new Game(players, messengers,
//                                               map, units, teams_info,
//                                               occupants, territories));
        game = std::unique_ptr<Game> (new Game(path, config,
                                               teams_info, players));
        game.get()->start();
        std::cout << "Game started" << std::endl;
        return true;
    }
    return false;
}

void Lobby::ready(Player* player) {
    if (players.size() >= 2) {
        bool any_not_ready = false;
        for (auto p: players) {
            if (!p->areYouReady()) {
                any_not_ready = true;
            }
        }
        if (!any_not_ready)
            all_ready = true;
    }
}



bool Lobby::addPlayer(Player* player) {
    if (!game_started) {
        if (players.size() < 4)
            players.push_back(player);

        std::string names_cmd = "names-";
        for (std::string name : get_player_names()) {
            names_cmd += name + "-";
        }

        for (Player *p : players) {
            p->getMessenger()->sendMessage(names_cmd);

        }
        teams.push_back(std::vector<std::string>());
        teams.back().push_back(player->getId());
        return (players.size() < 4);
    }
}

std::vector<std::string> Lobby::get_player_names() {
    std::vector<std::string> names;
    for (Player* p : players) {
        names.push_back(p->getId());
    }
    return names;
}

int Lobby::get_id() {
    return lobby_id;
}

void Lobby::unReady() {
    all_ready = false;
}

Lobby::~Lobby() {
    game.get()->shutDownGame();
    game.get()->join();
}

void Lobby::exitLobby(Player *player) {
    std::vector<Player *>::iterator it = players.begin();
    for (; it != players.end(); ++it) {
        if ((*it)->getId() == player->getId()) {
            players.erase(it);
            break;
        }
    }

    for (auto& t: teams) {
        std::vector<std::string>::iterator ito = t.begin();
        for (;ito != t.end(); ++ito) {
            if (*ito == player->getId()) {
                t.erase(ito);
                break;
            }
        }
    }

    std::string names_cmd = "names-";
    for (std::string name : get_player_names()) {
        names_cmd += name + "-";
    }

    for(Player* p : players) {
        p->getMessenger()->sendMessage(names_cmd);
    }
}

std::string Lobby::get_loaded_maps() {
    std::stringstream s;
    s << "mapsinfo-";
    for (auto map : maps) {
        s << map.first << "-";
    }
    return s.str();
}


void Lobby::load_maps() {
    pugi::xml_document doc;
    doc.load_file(config.c_str());
    pugi::xml_node cfg_root = doc.child("Config");
    pugi::xml_node maps_node = cfg_root.child("Maps");
    for (pugi::xml_node map : maps_node.children()) {
        maps[map.attribute("name").value()] = map.attribute("path").value();
    }
}
