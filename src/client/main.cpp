#include <gtkmm.h>
#include <iostream>
#include "ClientThread.h"
#include "GameBuilder.h"
#include "Game.h"
#include <split.h>

#define SUCCESSRETURNCODE 0
#define ERRORCODE 1
int main(int argc, char **argv) {
    try {
        auto app = Gtk::Application::create();

        GameBuilder builder;
        InitialWindow *window = builder.get_initial_window();
        app->run(*window);

        // Once the window closes, we fetch the socket
        std::shared_ptr<ServerMessenger> m = window->get_socket();
        std::string player_name = window->get_username();
        if (m) {
            ServerMessenger messenger = *m.get();

            // Starts the game
            Game g(builder, messenger, player_name);
        }
        return SUCCESSRETURNCODE;
    } catch (std::exception const &ex) {
        std::cerr << ex.what() << std::endl;
        return ERRORCODE;
    }
}
