#include <gtkmm.h>
#include <iostream>
#include "GameWindow.h"
#include "pugixml.hpp"
#include "MapGenerator.h"
#include <SDL2/SDL_mixer.h>


void play_sound() {
    // Init, open the audio channel
    Mix_Init(0);
    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024)==-1) {
        std::cout << "ERROR ON OPENING AUDIO" << Mix_GetError() << std::endl;
        return;
    }


    Mix_AllocateChannels(16);
    Mix_Chunk* sample = Mix_LoadWAV("test.wav");
    if (!sample) {
        std::cout << "ERROR ON PLAYING TEST.WAV " << Mix_GetError() << std::endl;
        return;
    }

    // Plays
    Mix_PlayChannel(-1, sample, -1);
}

int main (int argc, char **argv)
{
    //play_sound();

    auto app = Gtk::Application::create(argc, argv);
    MapGenerator generator(10, 10, 30, 0);
    generator.generate("map");
    GameWindow game_window;
    Gtk::Window* window = game_window.get_window();
    if (window)
    {
        return app->run(*window);
    }

    return 1;
}
