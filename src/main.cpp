#include <gtkmm.h>
#include <iostream>
#include "GameWindow.h"


int main (int argc, char **argv)
{
    auto app = Gtk::Application::create(argc, argv);

    GameWindow game_window;
    Gtk::Window* window = game_window.get_window();
    if (window)
    {
        return app->run(*window);
    }

    return 1;
}

