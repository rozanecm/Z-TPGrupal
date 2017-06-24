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

    bool play_again = false;
public:
    Game(GameBuilder& builder, ServerMessenger& server_messenger,
         const std::string& player_name);

    bool get_play_again_status();
private:
    void start_menu();

    void start_lobby() const;

    void start_game(const std::vector<std::string> &names);

    void results_screen(bool winner, bool loser);
};


#endif //Z_TPGRUPAL_GAME_H
