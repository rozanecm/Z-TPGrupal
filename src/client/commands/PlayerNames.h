#ifndef Z_TPGRUPAL_PLAYERNAMES_H
#define Z_TPGRUPAL_PLAYERNAMES_H


#include "Command.h"
#include "../windows/LobbyWindow.h"

class PlayerNames : public Command {
    LobbyWindow& lobby;
public:
    PlayerNames(LobbyWindow& lobby);
    void execute(const std::vector<std::string> &args);

};


#endif //Z_TPGRUPAL_PLAYERNAMES_H
