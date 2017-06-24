#ifndef Z_TPGRUPAL_GAME_H
#define Z_TPGRUPAL_GAME_H


#include "GameBuilder.h"
#include "ServerMessenger.h"
#include "windows/ResultWindow.h"

class Game {
    ServerMessenger &messenger;
    std::string me;

    // Monitors of shared assets, they'll be loaded when the game properly
    // starts
    MapMonitor mapMonitor;
    UnitsMonitor units_monitor;
    BuildingsMonitor buildingsMonitor;

    // Windows of different game stages
    MenuWindow* menu;
    LobbyWindow* lobby;
    GameWindow* game;
    ResultWindow* result;
public:
    Game(GameBuilder& builder, ServerMessenger& server_messenger,
         const std::string& player_name);

private:
    void start_menu();

    void start_lobby() const;

    void start_game(const std::vector<std::string> &names);
};


#endif //Z_TPGRUPAL_GAME_H
