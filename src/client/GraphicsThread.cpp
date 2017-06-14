#include <gtkmm/window.h>
#include "GraphicsThread.h"
#include "GameBuilder.h"
#include "BuildingsMonitor.h"
#include "MapMonitor.h"
#include "ServerMessenger.h"

void GraphicsThread::run() {
    auto app = Gtk::Application::create();

    GameBuilder game;
    GameWindow* window = game.get_window();

    if (window) {
        window->change_view_to_building();
        window->setResources(&unitsMonitor,
                             &buildingsMonitor,
                             &mapMonitor,
                             &messenger);
        app->run(*window);
        //todo check exit status. Couldn't find exit status codes,
        //so don't know what each number means
    }
}

GraphicsThread::GraphicsThread(UnitsMonitor &monitor,
                               BuildingsMonitor &buildingsMonitor,
                               MapMonitor &mapMonitor,
                               ServerMessenger &messenger) :
        unitsMonitor(monitor),
        buildingsMonitor(buildingsMonitor),
        mapMonitor(mapMonitor),
        messenger(messenger)
{
}
