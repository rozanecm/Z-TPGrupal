#include <gtkmm.h>
#include <iostream>
#include <SDL2/SDL_mixer.h>
#include "GraphicsThread.h"
#include "ClientThread.h"
#include <vector>
#include "Map.h"
#include "MapMonitor.h"

#define SUCCESSRETURNCODE 0

void play_sound() {
    // Init, open the audio channel
    Mix_Init(0);
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024)==-1) {
        std::cout << "ERROR ON OPENING AUDIO" << Mix_GetError() << std::endl;
        return;
    }


    Mix_AllocateChannels(16);
    Mix_Chunk* sample = Mix_LoadWAV("test.wav");
    if (!sample) {
        //todo throw exception
        std::cout <<"ERROR ON PLAYING TEST.WAV " << Mix_GetError() << std::endl;
        return;
    }

    // Plays
    Mix_PlayChannel(-1, sample, -1);
}

int main(int argc, char **argv) {
    play_sound();

    /* create map; bind with monitor */
    Map map;
    MapMonitor mapMonitor(map);
    /* create vector with players; bind with monitor */
    std::vector<Player> players;
    PlayersMonitor playerMonitor(players);
    /* create vector with buildings; bind with monitor */
    std::vector<Building> buildings;
    BuildingsMonitor buildingsMonitor(buildings);

    /* create graphics and client threads */
    GraphicsThread graphicsThread(argc, argv, playerMonitor, buildingsMonitor,
                                  mapMonitor);
    ClientThread clientThread(playerMonitor, buildingsMonitor, mapMonitor);

    /* start threads */
    clientThread.start();
    graphicsThread.start();

    /* join threads */
    clientThread.join();
    graphicsThread.join();

    return SUCCESSRETURNCODE;
}
