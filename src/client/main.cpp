#include <gtkmm.h>
#include <iostream>
#include <SDL2/SDL_mixer.h>
#include "GraphicsThread.h"
#include "ClientThread.h"
#include "GameBuilder.h"
#include <split.h>

#define SUCCESSRETURNCODE 0

void play_sound() {
    // Init, open the audio channel
    Mix_Init(0);
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1) {
        std::cout << "ERROR ON OPENING AUDIO" << Mix_GetError() << std::endl;
        return;
    }


    Mix_AllocateChannels(16);
    Mix_Chunk *sample = Mix_LoadWAV("test.wav");
    if (!sample) {
        //todo throw exception
        std::cout << "ERROR ON PLAYING TEST.WAV " << Mix_GetError()
                  << std::endl;
        return;
    }

    // Plays
    Mix_PlayChannel(-1, sample, -1);
}

void setup_lobby(ServerMessenger& m) {
    m.send("lobbyinfo");
    std::string response = m.receive();
    std::vector<std::string> args = split(response, '-');
    if (args.size() > 1) {
        m.send("getinlobby-0");
    } else {
        m.send("createlobby");
    }
    m.send("ready");
}

int main(int argc, char **argv) {
    try {
        // play_sound();

        /* create map; bind with monitor */
        Map map;
        MapMonitor mapMonitor(map);
        /* create vector with players; bind with monitor */
        std::vector<Unit> units;
        UnitsMonitor unitsrMonitor(units);

        /* create vector with buildings; bind with monitor */
        std::vector<Building> buildings;
        BuildingsMonitor buildingsMonitor(buildings);

        auto app = Gtk::Application::create();

        GameBuilder builder;
        InitialWindow *window = builder.get_initial_window();
        app->run(*window);

        // Once the window closes, we fetch the socket
        std::shared_ptr<ServerMessenger> m = window->get_socket();
        if (m) {
            ServerMessenger messenger = *m.get();

            GameWindow *gwindow = builder.get_window();


            LobbyWindow* lobby = builder.get_lobby_window();
            lobby->set_messenger(messenger);
            app.clear();
            app = Gtk::Application::create();
            setup_lobby(messenger);
            ClientThread clientThread(unitsrMonitor, buildingsMonitor,
                                      mapMonitor,
                                      messenger, *lobby, *gwindow);
            clientThread.start();

            app->run(*lobby);
            // HARDCODED DEBUG MESSAGES TO START A GAME


            GraphicsThread graphicsThread(unitsrMonitor, buildingsMonitor,
                                          mapMonitor, messenger, *gwindow);


            graphicsThread.start();
            graphicsThread.join();

            /* once graphics join (window closes), we kill client thread */
            clientThread.finish();
            clientThread.join();
        }

        return SUCCESSRETURNCODE;
    } catch (std::exception const &ex) {
        std::cerr << ex.what() << std::endl;
        return SUCCESSRETURNCODE;
    }
}
