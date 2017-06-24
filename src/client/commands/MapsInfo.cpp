#include "MapsInfo.h"

MapsInfo::MapsInfo(LobbyWindow &lobby) : lobby(lobby) {
}

void MapsInfo::execute(const std::vector<std::string> &args) {
    std::stringstream s;
    for (const std::string& map : args) {
        s << map << std::endl;
    }
    lobby.update_maps(s.str());
}
