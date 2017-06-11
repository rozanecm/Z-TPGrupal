#include <gtkmm.h>
#include <iostream>
#include <SDL2/SDL_mixer.h>
#include "GraphicsThread.h"
#include "ClientThread.h"
#include <vector>
#include "Map.h"
#include "MapMonitor.h"
#include "../common/messenger.h"
#include "GameBuilder.h"

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
    // play_sound();

    /* create map; bind with monitor */
    Map map;
    MapMonitor mapMonitor(map);
    /* create vector with players; bind with monitor */
    std::vector<Player> players;
    PlayersMonitor playerMonitor(players);
    /* create vector with buildings; bind with monitor */
    std::vector<Building> buildings;
    BuildingsMonitor buildingsMonitor(buildings);

    auto app = Gtk::Application::create();

    GameBuilder builder;
    InitialWindow* window = builder.get_initial_window();
    app->run(*window);
    // Once the window closes, we fetch the socket
    std::shared_ptr<Socket> s = window->get_socket();
    if (s) {
        ServerMessenger messenger(*s.get());

        ClientThread clientThread(playerMonitor, buildingsMonitor, mapMonitor,
                                  messenger);
        clientThread.start();

        GraphicsThread graphicsThread(playerMonitor, buildingsMonitor,
                                      mapMonitor, messenger);

        // HARDCODED DEBUG MESSAGES TO START A GAME
        messenger.send("create-lobby");
        messenger.send("ready");
        messenger.send("start-game");

        graphicsThread.start();
        graphicsThread.join();

        /* once graphics join (window closes), we kill client thread */
        clientThread.finish();
        clientThread.join();
    }

    return SUCCESSRETURNCODE;
}
