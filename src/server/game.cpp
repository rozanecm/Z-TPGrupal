//
// Created by rodian on 29/05/17.
//

#include "game.h"

// get terrain map from map loader
Game::Game(std::vector<Player> players, int x, int y, int size, int height,
           std::vector<std::vector<Cell>> terrain_map) : players(players),
                                           control(), map(5,5,9,9,terrain_map){}

