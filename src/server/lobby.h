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
    std::unique_ptr<Game> game;
    std::vector<std::vector<std::string>> teams;
    std::map<std::string, std::string> maps;
    std::string& config;
public:
    Lobby(int id, std::string& config);

    bool addPlayer(Player* player);

    bool startGame(const std::string& map_name);

    void ready(Player* player);

    std::vector<std::string> get_player_names();

    void unReady();

    int get_id();

    void exitLobby(Player* player);

    std::string get_loaded_maps();

    void load_maps();

    ~Lobby();
};


#endif //Z_TPGRUPAL_LOBBY_H
