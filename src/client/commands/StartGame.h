#ifndef Z_TPGRUPAL_STARTGAME_H
#define Z_TPGRUPAL_STARTGAME_H


#include "Command.h"
#include "../windows/GameWindow.h"
#include "../ServerMessenger.h"
#include "../windows/LobbyWindow.h"

class StartGame : public Command {
    ServerMessenger &m;
    LobbyWindow& lobby;
    GameWindow& window;

public:

    StartGame(ServerMessenger& m, LobbyWindow& lobby, GameWindow& window);
    void execute(const std::vector<std::string> &args);

};


#endif //Z_TPGRUPAL_STARTGAME_H
