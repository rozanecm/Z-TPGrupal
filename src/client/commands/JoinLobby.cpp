#include "JoinLobby.h"

#define STATUS 0

#define OK "ok"
#define ERROR "error"
JoinLobby::JoinLobby(MenuWindow &menu, LobbyWindow &lobby) :
        menu(menu),
        lobby(lobby)
{
}

void JoinLobby::execute(const std::vector<std::string> &args) {
    if (args[STATUS] == OK) {
        menu.hide();
    }
}
