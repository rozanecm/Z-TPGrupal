#include <gtkmm.h>
#include <iostream>
#include "GameWindow.h"
#include "../libs/xml/pugixml.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

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

void play_sound() {
    int init = Mix_Init(0);
    if (!init) {
        std::cout << "ERROR ON SDL MIXER LIBRARY" << std::endl;
        return;
    }
    Mix_Chunk* sample = Mix_LoadWAV("test.wav");
    if (!sample) {
        std::cout << "ERROR ON PLAYING TEST.WAV" << std::endl;
        return;
    }
    Mix_PlayChannel(-1, sample, -1);
}

int main (int argc, char **argv)
{
    play_sound();
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
