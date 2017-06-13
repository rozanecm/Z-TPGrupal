//
// Created by rodian on 29/05/17.
//

#ifndef Z_TPGRUPAL_GAME_H
#define Z_TPGRUPAL_GAME_H


#include "map.h"
#include "controlUnit.h"
#include "player.h"
class ControlUnit;
class Player;

class Game: public Thread {
private:
    std::vector<Messenger*> players;
    ControlUnit control;
    std::shared_ptr<Map> map;
//    std::vector<Unit>& all_units;
//    std::vector<Occupant>& all_occupants;

public:
    Game(std::vector<Messenger *> players, std::shared_ptr<Map> map,
             std::map<int, Unit> &units, std::vector<Team>& teams_info,
             std::vector<Occupant*>& occupants);

//    Game(std::vector<Player*> players, std::shared_ptr<Map> map);

    void addBuildings(std::vector<Occupant> buildings);

    void run();

    void shutDownGame();
};


#endif //Z_TPGRUPAL_GAME_H
