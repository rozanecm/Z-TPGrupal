#include <gtkmm/window.h>
#include "GraphicsThread.h"
#include "GameBuilder.h"

void GraphicsThread::run() {
    auto app = Gtk::Application::create();

    window.setResources(&unitsMonitor, &buildingsMonitor, &mapMonitor,
                        &messenger, owner);
    
    app->run(window);
    //todo check exit status. Couldn't find exit status codes,
    //so don't know what each number means
}

GraphicsThread::GraphicsThread(UnitsMonitor &monitor,
                               BuildingsMonitor &buildingsMonitor,
                               MapMonitor &mapMonitor,
                               ServerMessenger &messenger,
                               GameWindow &window, const std::string &owner) :
        unitsMonitor(monitor),
        buildingsMonitor(buildingsMonitor),
        mapMonitor(mapMonitor),
        messenger(messenger),
        window(window),
        owner(owner){
}
