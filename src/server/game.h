//
// Created by rodian on 29/05/17.
//

#ifndef Z_TPGRUPAL_GAME_H
#define Z_TPGRUPAL_GAME_H


#include "map.h"
#include "controlUnit.h"
#include "player.h"
class ControlUnit;

class Game {
private:
    std::vector<Messenger*> players;
    ControlUnit control;
    Map map;

public:
    Game(std::vector<Messenger*> players, int x, int y, int width, int height,
         std::vector<std::vector<Cell>> terrain_map,
         std::vector<Occupant>& occupants,
         std::vector<Unit>& units);

    void addBuildings(std::vector<Occupant> buildings);
    void startGame();
};


#endif //Z_TPGRUPAL_GAME_H
