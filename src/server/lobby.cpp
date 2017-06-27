//
// Created by rodian on 29/05/17.
//

#include <sstream>
#include "lobby.h"

Lobby::Lobby(int id, std::string& config) : lobby_id(id),
                                            config(config),
                                            all_ready(false),
                                            game_started(false) {
    load_maps();
}

void Lobby::startGame(const std::string& map_name) {
    Lock l(m);
    if(all_ready) {
        //start game
        game_started = true;
        auto path_it = maps.find(map_name);
        if (path_it != maps.end()) {
            std::string path = path_it->second;

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
                Team new_team(playersInfo, i);
                teams_info.push_back(new_team);
            }

            for (auto p: players) {
                p->getInGame();
            }

            game = new Game(path, config, teams_info, players);
            game->start();
        }
    }
}

void Lobby::ready() {
    Lock l(m);
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
    } else {
        return false;
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
    Lock l(m);
    all_ready = false;
}

void Lobby::exitLobby(Player *player) {
    Lock l(m);
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

void Lobby::shutDown() {
    game->shutDownGame();
    game->join();
}

Lobby::~Lobby() {}

bool Lobby::haveGameFinished() {
    if (game_started) {
        return game->gameHaveFinished();
    } else {
        return false;
    }
}

void Lobby::disconectPlayer(Player *player) {
    std::vector<Player*>::iterator it = players.begin();
    for(;it != players.end();++it) {
        if ((*it)->getId() == player->getId()) {
            if (game_started)
                this->game->disconectPlayer(player->getId());
            players.erase(it);
            break;
        }
    }
}
