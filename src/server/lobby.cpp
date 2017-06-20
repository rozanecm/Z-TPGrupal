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
        for (auto p: players) {
            messengers.push_back(p->getMessenger());
            p->getInGame();
        }

        std::vector<Occupant*>& occupants = map->getOccupants();
        std::map<int, Unit*> units;
        /// estas unidades creadas son para realizar tests
        Size r_size(32,32,6,6);
        Size range(32,32,12,12);

//        Size r_size(415,431,6,6);
//        Size range(415,431,12,12);

        Size j_size(10,10,3,3);
        Size j_range(10,10,6,6);
        Compass* r_compass = new Compass(*map, r_size,204, 4);
        Compass* j_compass = new Compass(*map, j_size,208, 4);
        Weapon weapon("laser",25,20,false,r_size);
        Unit* robot = new Unit(204,400,"grunt",4,r_size,range,*r_compass,weapon,2);
        Unit* jeep = new Unit(208,400,"jeep",5,j_size,j_range,*j_compass,weapon,2);

        robot->changeTeam(players[0]->getId());
        if (players.size() >= 2)
            jeep->changeTeam(players[1]->getId());

        units.insert(std::pair<int,Unit*>(204,robot));
        units.insert(std::pair<int,Unit*>(208,jeep));
        occupants.push_back((Occupant*) robot);
        occupants.push_back((Occupant*) jeep);
        ///
        game = std::unique_ptr<Game> (new Game(players,messengers, map, units,
                                               teams_info,
                                               occupants));
        game.get()->start();
        std::cout << "Game started" << std::endl;
    }
}

void Lobby::ready(Player* player) {
    bool any_not_ready = false;
    for (auto p: players) {
        if (!p->areYouReady()) {
            any_not_ready = true;
        }
    }
    if (!any_not_ready)
        all_ready = true;
    player->getMessenger()->sendMessage("ready-to-go");
}

bool Lobby::addPlayer(Player* player) {
    if(players.size() < 4)
        players.push_back(player);
    if (teams.size() == 0) {
        teams.push_back(std::vector<std::string>());
    }
    teams.back().push_back(player->getId());
    return (players.size() < 4);
}

Lobby::~Lobby() {
    game.get()->shutDownGame();
    game.get()->join();
}
