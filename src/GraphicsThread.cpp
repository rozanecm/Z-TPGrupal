#include <gtkmm/window.h>
#include "GraphicsThread.h"
#include "GameBuilder.h"

void GraphicsThread::run() {
    auto app = Gtk::Application::create(argc, argv);

    GameBuilder game;
    GameWindow* window = game.get_window();

    if (window) {
        window->change_view_to_building();

        app->run(*window);
        //todo check exit status. Couldn't find exit status codes,
        //so don't know what each number means
    }
}

GraphicsThread::GraphicsThread(int argc, char **argv) : argc(argc), argv(argv){

}
