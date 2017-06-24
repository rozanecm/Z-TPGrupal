#ifndef Z_TPGRUPAL_LOBBYINFO_H
#define Z_TPGRUPAL_LOBBYINFO_H


#include "Command.h"
#include "../windows/MenuWindow.h"

class LobbyInfo : public Command {
    MenuWindow& menu;
public:
    LobbyInfo(MenuWindow& menu);
    void execute(const std::vector<std::string> &args);

};


#endif //Z_TPGRUPAL_LOBBYINFO_H
