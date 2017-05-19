#include <gtkmm/application.h>
#include "GameWindow.h"

int main (int argc, char *argv[]) {
    auto app = Gtk::Application::create(argc, argv);

    GameWindow gameWindow;

    return app->run(gameWindow);
}
