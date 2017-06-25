#include "Game.h"
#include "ClientThread.h"

void Game::start_game(const std::vector<std::string> &names) {
    game->update_players(names);
    game->update_name(me);
    // Start up the game
    game->setResources(&units_monitor, &buildingsMonitor,
                          &mapMonitor, &messenger, me);

    auto app = Gtk::Application::create();
    app->run(*game);
}

Game::Game(GameBuilder &builder, ServerMessenger &server_messenger,
           const std::string& player_name, MapMonitor& map,
           UnitsMonitor& units, BuildingsMonitor& buildings) :
    messenger(server_messenger),
    me(player_name),
    menu(builder.get_menu_window()),
    lobby(builder.get_lobby_window()),
    game(builder.get_window()),
    result(builder.get_result_window()),
    mapMonitor(map),
    units_monitor(units),
    buildingsMonitor(buildings)
{

    start_menu();

    if (menu->joined_lobby()) {
        start_lobby();
        if (lobby->game_started()) {
            std::vector<std::string> names = lobby->get_player_names();
            mapMonitor.update_players(names);
            start_game(names);
        }
    }

    bool winner = mapMonitor.is_winner();
    bool loser = mapMonitor.is_loser();
    results_screen(winner, loser);
}

void Game::start_lobby() const {
    lobby->set_messenger(messenger);
    auto app = Gtk::Application::create();
    app->run(*lobby);
}

void Game::start_menu() {
    menu->load_messenger(&messenger);
    messenger.send("lobbyinfo");
    auto app = Gtk::Application::create();
    app->run(*menu);
}

void Game::results_screen(bool winner, bool loser) {
    if (!winner && !loser) { // Played closed the window before game was over
        play_again = false;
        return;
    } else if (winner) {
        result->display_win_screen();
    } else {
        result->display_lose_screen();
    }

    auto app = Gtk::Application::create();
    app->run(*result);
    bool play_again = result->go_back_to_menu();
    if (play_again) {
        messenger.send("returntomenu");
    }
    this->play_again = play_again;
}

bool Game::get_play_again_status() {
    return play_again;
}

Game::~Game() { // Game finishes, clear assets
    units_monitor.clear();
    mapMonitor.clear();
    buildingsMonitor.clear();
}
