//
// Created by rodian on 29/05/17.
//

#include "lobby.h"
#define PATH "maps/map.xml"

Lobby::Lobby(int id, std::string& config) : lobby_id(id),
                                            config(config),
                                            all_ready(false) {

}

void Lobby::startGame() {
    std::cout << "Beginning game" << std::endl;
    if(all_ready){
        //start game
        std::string path = PATH;
        MapLoader maploader(path, config);
        std::shared_ptr<Map> map = maploader.get_map();

        std::vector<Occupant*>& occupants = map->getOccupants();
        std::map<int, Unit*> units;

        // build teams
        std::vector<Factory*> forts = maploader.get_forts();
        std::vector<Team> teams_info;
        for (int i = 0; i < teams.size(); ++i) {
            std::vector<PlayerInfo> playersInfo;
            for (int j = 0; j < teams[i].size(); ++j) {
                Factory* fortress = forts.back();
                PlayerInfo new_player(teams[i][j],fortress);
                for (auto p: players) {
                    if (p->getId() == teams[i][j])
                        new_player.addMessenger(p->getMessenger());
                }
                occupants.push_back((Occupant*) fortress);
                forts.pop_back();
                playersInfo.push_back(new_player);
            }
            Team new_team(playersInfo,i);
            teams_info.push_back(new_team);
        }

        // get messengers
        std::vector<Messenger *> messengers;
        for (auto p: players) {
            messengers.push_back(p->getMessenger());
            p->getInGame();
        }

        /// estas unidades creadas son para realizar tests
        Size r_size(0,0,3,3);
        Size range(0,0,12,12);
        Size laser_size(0,0,3,1);
//        Size r_size(415,431,6,6);
//        Size range(415,431,12,12);

        Size j_size(20,20,3,3);
        Size j_range(20,20,6,6);
        Compass* r_compass = new Compass(*map, r_size,204, 4);
        Compass* j_compass = new Compass(*map, j_size,208, 4);
        std::string w_type = "laser";
        Weapon weapon(w_type,25,20,false,laser_size);
        Unit* robot = new Unit(204,400,"grunt",4,r_size,range,*r_compass,weapon,2);
        Unit* jeep = new Unit(208,400,"jeep",5,j_size,j_range,*j_compass,
                              weapon,2);

        robot->changeTeam(players[0]->getId());
        if (players.size() >= 2)
            jeep->changeTeam(players[1]->getId());

        units.insert(std::pair<int,Unit*>(204,robot));
        units.insert(std::pair<int,Unit*>(208,jeep));
        occupants.push_back((Occupant*) robot);
        occupants.push_back((Occupant*) jeep);
        //////////////////////////////
        std::vector<Territory *> territories = maploader.get_territories();
        game = std::unique_ptr<Game> (new Game(players, messengers,
                                               map, units, teams_info,
                                               occupants, territories));
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
//    player->getMessenger()->sendMessage("ready-to-go");
}

bool Lobby::addPlayer(Player* player) {
    if (players.size() < 4)
        players.push_back(player);
    if (teams.size() == 0) {
        teams.push_back(std::vector<std::string>());
    }

    std::string names_cmd = "names-";
    for (std::string name : get_player_names()) {
        names_cmd += name + "-";
    }

    for(Player* p : players) {
        p->getMessenger()->sendMessage(names_cmd);

    }
    teams.back().push_back(player->getId());
    return (players.size() < 4);
}

Lobby::~Lobby() {
    game.get()->shutDownGame();
    game.get()->join();
}

std::vector<std::string> Lobby::get_player_names() {
    std::vector<std::string> names;
    for (Player* p : players) {
        names.push_back(p->getId());
    }
    return names;
}
