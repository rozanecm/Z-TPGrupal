#include <gtkmm/window.h>
#include "GraphicsThread.h"
#include "GameBuilder.h"
#include "PlayersMonitor.h"
#include "BuildingsMonitor.h"
#include "MapMonitor.h"
#include "ServerMessenger.h"

void GraphicsThread::run() {
    auto app = Gtk::Application::create();

    GameBuilder game(playerMonitor, buildingsMonitor, mapMonitor, messenger);
    GameWindow* window = game.get_window();

    if (window) {
        window->change_view_to_building();

        app->run(*window);
        //todo check exit status. Couldn't find exit status codes,
        //so don't know what each number means
    }
}

GraphicsThread::GraphicsThread(PlayersMonitor &monitor,
                               BuildingsMonitor &buildingsMonitor,
                               MapMonitor &mapMonitor,
                               ServerMessenger &messenger) :
        playerMonitor(monitor),
        buildingsMonitor(buildingsMonitor),
        mapMonitor(mapMonitor),
        messenger(messenger)
{
}
