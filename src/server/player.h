//
// Created by rodian on 29/05/17.
//

#ifndef Z_TPGRUPAL_PLAYER_H
#define Z_TPGRUPAL_PLAYER_H

#include <iostream>
#include "../messenger.h"
#include "../Thread.h"
#include "command.h"

class Player: public Thread {
private:
    Messenger* messenger;
    std::string id;
    int color;
    bool conected, playing;
    std::vector<Command>* cmds;

public:
    Player(std::string id, Messenger* messenger, std::vector<Command>* cmds);

    void run();
};


#endif //Z_TPGRUPAL_PLAYER_H
