//
// Created by rodian on 29/05/17.
//

#ifndef Z_TPGRUPAL_PLAYER_H
#define Z_TPGRUPAL_PLAYER_H

#include <iostream>
#include "../messenger.h"
#include "../Thread.h"
#include "command.h"
#include "menu.h"
#include "lobby.h"

class Command;
class ControlUnit;
class Lobby;
class Menu;

class Player: public Thread {
private:
    Messenger* messenger;
    std::string id;
    int color;
    bool conected,on_menu,on_lobby ,playing;
    std::vector<Command*> cmds;
    ControlUnit* control;
    Lobby* lobby;
    Menu& menu;

public:
    Player(std::string id, Messenger* messenger, ControlUnit* control, Menu& menu);

    Player(std::string id, Messenger* messenger, Menu& menu);

    void run();

    void updateInfo(std::string& info);

    void addLobby(Lobby* lobby);

    void addControlUnit(ControlUnit* control);

    Messenger* getMessenger();

private:
    void processMenuCommands(std::string& cmd);

    void processLobbyCommands(std::string& cmd);
};


#endif //Z_TPGRUPAL_PLAYER_H
