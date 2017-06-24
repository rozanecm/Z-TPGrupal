#include <gtkmm.h>
#include <iostream>
#include "GraphicsThread.h"
#include "ClientThread.h"
#include "GameBuilder.h"
#include <split.h>

#define SUCCESSRETURNCODE 0

int main(int argc, char **argv) {
    try {
        /* create map; bind with monitor */
        Map map;
        MapMonitor mapMonitor(map);
        /* create vector with players; bind with monitor */
        std::vector<Unit> units;
        UnitsMonitor units_monitor(units);

        /* create vector with buildings; bind with monitor */
        std::vector<Building> buildings;
        BuildingsMonitor buildingsMonitor(buildings);

        auto app = Gtk::Application::create();

        GameBuilder builder;
        InitialWindow *window = builder.get_initial_window();
        app->run(*window);

        // Once the window closes, we fetch the socket
        std::shared_ptr<ServerMessenger> m = window->get_socket();
        if (m) {
            ServerMessenger messenger = *m.get();

            // Start thread that handles server communication
            ClientThread clientThread(units_monitor, buildingsMonitor,
                                      mapMonitor, messenger, builder);
            clientThread.start();

            MenuWindow* menu = builder.get_menu_window();
            menu->load_messenger(&messenger);
            messenger.send("lobbyinfo");
            app = Gtk::Application::create();
            app->run(*menu);

            LobbyWindow* lobby = builder.get_lobby_window();
            lobby->set_messenger(messenger);

            app = Gtk::Application::create();
            app->run(*lobby);


            GameWindow *gwindow = builder.get_window();
            gwindow->update_name(window->get_username());
            gwindow->update_players(lobby->get_player_names());
            clientThread.update_player_names(lobby->get_player_names());

            // Start up the game
            GraphicsThread graphicsThread(units_monitor, buildingsMonitor,
                                          mapMonitor, messenger, *gwindow,
                                          window->get_username());


            graphicsThread.start();
            graphicsThread.join();

            /* once graphics join (window closes), we kill client thread */
            clientThread.finish();
            clientThread.join();
        }

        return SUCCESSRETURNCODE;
    } catch (std::exception const &ex) {
        std::cerr << ex.what() << std::endl;
        return SUCCESSRETURNCODE;
    }
}
