//
// Created by rodian on 29/05/17.
//

#ifndef Z_TPGRUPAL_LOBBY_H
#define Z_TPGRUPAL_LOBBY_H


#include "player.h"
#include "game.h"
#include "MapLoader.h"
class Game;
class Player;

class Lobby {
private:
    int lobby_id;
    bool all_ready;
    std::vector<Player*> players;
    std::unique_ptr<Game> game;

public:
    Lobby(int id);

    bool addPlayer(Player* player);

    void startGame();

    void ready(Player* player);

    ~Lobby();
};


#endif //Z_TPGRUPAL_LOBBY_H
