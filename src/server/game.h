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
    std::vector<Player*> players;
//    ControlUnit control;
    Map map;
//    std::vector<Unit>& all_units;
//    std::vector<Occupant>& all_occupants;

public:
    Game(std::vector<Player*> players, Map& map,
         std::map<int,Unit>& units);

    Game(std::vector<Player*> players, Map& map);

    void addBuildings(std::vector<Occupant> buildings);

    void run();

    void shutDownGame();
};


#endif //Z_TPGRUPAL_GAME_H
