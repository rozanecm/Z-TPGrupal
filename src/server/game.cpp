//
// Created by rodian on 29/05/17.
//

#include "game.h"

// get terrain map from map loader
Game::Game(std::vector<Messenger*> players, int x, int y, int width, int height,
           std::vector<std::vector<Cell>> terrain_map,
           std::vector<Occupant>& occupants,
           std::vector<Unit>& units) : players(players),
           control(players,units,occupants),
           map(x,y,width,height,terrain_map,occupants){}

void Game::startGame() {
    control.run();
}

void Game::addBuildings(std::vector<Occupant> buildings) {

}

