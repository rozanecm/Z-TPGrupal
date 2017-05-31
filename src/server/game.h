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
    Game(std::vector<Messenger*> players, int x, int y, int size, int height,
         std::vector<std::vector<Cell>> terrain_map);

    void startGame();
};


#endif //Z_TPGRUPAL_GAME_H