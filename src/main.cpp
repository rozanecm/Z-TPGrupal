#include <gtkmm.h>
#include <iostream>
#include "GameWindow.h"
#include "xml/pugixml.hpp"
#include <SDL2>
#include "sdl/SDL_mixer.h"

void xml_play() {
    pugi::xml_document doc;
    doc.load_file("src/map.xml");

    pugi::xml_node_iterator row = doc.children().begin();
    for (; row != doc.children().end(); ++row) {
        pugi::xml_node_iterator cell = row->children().begin();
        for (; cell != row->children().end(); ++cell) {
            std::cout << "Name: " << cell->name() << std::endl;
            std::cout << "Key: " << cell->attribute("key").value() << std::endl;
        }
    }
}

int main (int argc, char **argv)
{
    xml_play();

    auto app = Gtk::Application::create(argc, argv);

    GameWindow game_window;
    Gtk::Window* window = game_window.get_window();
    if (window)
    {
        return app->run(*window);
    }

    return 1;
}

