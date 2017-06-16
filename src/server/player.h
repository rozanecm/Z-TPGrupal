//
// Created by rodian on 29/05/17.
//

#ifndef Z_TPGRUPAL_PLAYER_H
#define Z_TPGRUPAL_PLAYER_H

#include <iostream>
#include "../common/messenger.h"
#include "../common/Thread.h"
#include "menu.h"
#include "lobby.h"

class CommandMonitor;
class ControlUnit;
class Lobby;
class Menu;
class Game;

class Player: public Thread {
private:
    Messenger* messenger;
    std::string id;
    int color;
    bool conected,on_menu,on_lobby ,playing;
    CommandMonitor* commands;
    ControlUnit* control;
    Lobby* lobby;
    Menu& menu;

public:
    Player(Messenger* messenger, ControlUnit* control, Menu& menu);

    Player(Messenger *messenger, Menu &menu, std::string& id);

    void run();

    void updateInfo(std::string& info);

    void addLobby(Lobby* lobby);

    void addControlUnit(ControlUnit* control, CommandMonitor* commands);

    Messenger* getMessenger();

    void shutDown();

    std::string getId() const;

    ~Player();

private:
    void processMenuCommands(std::string& cmd);

    void processLobbyCommands(std::string& cmd);
};


#endif //Z_TPGRUPAL_PLAYER_H
