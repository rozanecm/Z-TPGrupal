//
// Created by rodian on 29/05/17.
//

#include "lobby.h"
#define PATH "maps/map.xml"

Lobby::Lobby(int id, std::string& config) : lobby_id(id),
                                            config(config),
                                            all_ready(false) {
    this->players = new std::vector<Player*>;
}

void Lobby::startGame() {
    std::cout << "Beginning game" << std::endl;
    if(all_ready){
        //start game
        MapLoader maploader(PATH, config);
        std::shared_ptr<Map> map = maploader.get_map();

        // build teams
        std::vector<Factory*> forts = maploader.get_forts();
        std::vector<Team> teams_info;
        for (int i = 0; i < teams.size(); ++i) {
            std::vector<PlayerInfo> players;
            for (int j = 0; j < teams[i].size(); ++j) {
                Factory* fortress = forts.back();
                PlayerInfo new_player(teams[i][j],*fortress);
                forts.pop_back();
                players.push_back(new_player);
            }
            Team new_team(players,i);
            teams_info.push_back(new_team);
        }

        // get messengers
        std::vector<Messenger *> messengers;
        for (auto p: *players) {
            messengers.push_back(p->getMessenger());
        }

        std::vector<Occupant*>& occupants = map->getOccupants();
        std::map<int, Unit> units;
        game = std::unique_ptr<Game> (new Game(messengers, map, units,
                                               teams_info,
                                               occupants));
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
    if(players->size() < 4)
        players->push_back(player);
    if (teams.size() == 0) {
        teams.push_back(std::vector<std::string>());
    }
    teams.back().push_back(player->getId());
    return (players->size() < 4);
}

Lobby::~Lobby() {
    game.get()->shutDownGame();
    game.get()->join();
}
