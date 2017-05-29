//
// Created by rodian on 29/05/17.
//

#ifndef Z_TPGRUPAL_GAME_H
#define Z_TPGRUPAL_GAME_H


#include "map.h"
#include "controlUnit.h"
#include "player.h"

class Game {
private:
    std::vector<Player> players;
    ControlUnit control;
    Map map;

public:
    Game(std::vector<Player> players, int x, int y, int size, int height,
         std::vector<std::vector<Cell>> terrain_map);
};


#endif //Z_TPGRUPAL_GAME_H
