#ifndef Z_TPGRUPAL_JOINLOBBY_H
#define Z_TPGRUPAL_JOINLOBBY_H


#include "Command.h"
#include "../windows/MenuWindow.h"
#include "../windows/LobbyWindow.h"

class JoinLobby : public Command {
    MenuWindow& menu;
    LobbyWindow& lobby;
public:
    JoinLobby(MenuWindow& menu, LobbyWindow& lobby);
    void execute(const std::vector<std::string> &args);
};


#endif //Z_TPGRUPAL_JOINLOBBY_H
