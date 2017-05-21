#include <gtkmm.h>
#include <iostream>
#include <pugixml.hpp>
#include <SDL2/SDL_mixer.h>
#include "GameBuilder.h"
#include "GameWindow.h"
#include "GraphicsThread.h"


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
    // play_sound();

    GraphicsThread graphicsThread(argc, argv);
    graphicsThread.start();

    graphicsThread.join();

    return 1;
}
