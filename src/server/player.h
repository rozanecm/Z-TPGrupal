//
// Created by rodian on 29/05/17.
//

#ifndef Z_TPGRUPAL_PLAYER_H
#define Z_TPGRUPAL_PLAYER_H

#include <iostream>
#include "../messenger.h"
#include "../Thread.h"
#include "command.h"
class Command;
class ControlUnit;


class Player: public Thread {
private:
    Messenger* messenger;
    std::string id;
    int color;
    bool conected, playing;
    std::vector<Command*> cmds;
    ControlUnit* control;
//    Menu& menu;

public:
    Player(std::string id, Messenger* messenger, ControlUnit* control/*, Menu& menu*/);

    Player(std::string id, Messenger* messenger/*, Menu& menu*/);

    void addControlUnit(ControlUnit* control);

    void run();

    void updateInfo(std::string& info);
};


#endif //Z_TPGRUPAL_PLAYER_H
