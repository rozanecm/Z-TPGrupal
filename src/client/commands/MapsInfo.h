#ifndef Z_TPGRUPAL_MAPSINFO_H
#define Z_TPGRUPAL_MAPSINFO_H


#include "Command.h"
#include "../windows/LobbyWindow.h"

class MapsInfo : public Command {
    LobbyWindow& lobby;
public:
    MapsInfo(LobbyWindow& lobby);
    void execute(const std::vector<std::string> &args);

};


#endif //Z_TPGRUPAL_MAPSINFO_H
