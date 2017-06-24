#include "LobbyInfo.h"

LobbyInfo::LobbyInfo(MenuWindow &menu) : menu(menu) {
}

void LobbyInfo::execute(const std::vector<std::string> &args) {
    menu.update_lobbies(args);
}
