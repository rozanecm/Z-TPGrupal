#ifndef Z_TPGRUPAL_JOINLOBBY_H
#define Z_TPGRUPAL_JOINLOBBY_H


#include "Command.h"
#include "../windows/MenuWindow.h"
#include "../windows/LobbyWindow.h"

class JoinLobby : public Command {
    MenuWindow& menu;
    LobbyWindow& lobby;
    ServerMessenger& messenger;
public:
    JoinLobby(MenuWindow& menu, LobbyWindow& lobby, ServerMessenger& messenger);
    void execute(const std::vector<std::string> &args);
};


#endif //Z_TPGRUPAL_JOINLOBBY_H
