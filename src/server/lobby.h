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
    bool all_ready, game_started;
    std::vector<Player*> players;
    Game* game;
    std::vector<std::vector<std::string>> teams;
    std::map<std::string, std::string> maps;
    std::mutex& m;
    std::string& config;
public:
    Lobby(int id, std::string& config, std::mutex& m);

    bool addPlayer(Player* player);

    void startGame(const std::string& map_name);

    void ready();

    std::vector<std::string> get_player_names();

    void unReady();

    int get_id();

    void exitLobby(Player* player);

    std::string get_loaded_maps();

    void load_maps();

    void shutDown();

    bool haveGameFinished();

    ~Lobby();
};


#endif //Z_TPGRUPAL_LOBBY_H
