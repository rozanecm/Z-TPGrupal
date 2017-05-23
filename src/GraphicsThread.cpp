#include <gtkmm/window.h>
#include "GraphicsThread.h"
#include "GameBuilder.h"
#include "PlayersMonitor.h"
#include "BuildingsMonitor.h"
#include "MapMonitor.h"

void GraphicsThread::run() {
    auto app = Gtk::Application::create(argc, argv);

    GameBuilder game(playerMonitor, buildingsMonitor, mapMonitor);
//    Gtk::Window* window = game.get_window();
//    game.change_view_to_building();
    GameWindow* window = game.get_window();

    if (window) {
        window->change_view_to_building();

        app->run(*window);
        //todo check exit status. Couldn't find exit status codes,
        //so don't know what each number means
    }
}

GraphicsThread::GraphicsThread(int argc, char **argv,
                               PlayersMonitor &monitor,
                               BuildingsMonitor &buildingsMonitor,
                               MapMonitor &mapMonitor)
        : argc(argc),
          argv(argv),
          playerMonitor(monitor),
          buildingsMonitor(buildingsMonitor),
          mapMonitor(mapMonitor){
}
