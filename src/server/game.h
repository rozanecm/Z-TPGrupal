//
// Created by rodian on 29/05/17.
//

#ifndef Z_TPGRUPAL_GAME_H
#define Z_TPGRUPAL_GAME_H


#include "map.h"
#include "controlUnit.h"
#include "player.h"
#include "commandMonitor.h"

class ControlUnit;
class Player;

class Game: public Thread {
private:
    std::mutex m;
    CommandMonitor commands;
    std::map<int,Unit*> all_units;
    std::vector<Player*> players;
    ControlUnit control;
    std::shared_ptr<Map> map;
//    std::vector<Occupant>& all_occupants;

public:
    Game(std::vector<Player *> players, std::vector<Messenger *> msgr,
         std::shared_ptr<Map> map, std::map<int, Unit*> units,
         std::vector<Team> teams_info, std::vector<Occupant*> occupants);

//    Game(std::vector<Player*> players, std::shared_ptr<Map> map);

    void addBuildings(std::vector<Occupant> buildings);

    void run();

    void shutDownGame();
};


#endif //Z_TPGRUPAL_GAME_H
