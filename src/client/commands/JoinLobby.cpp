#include "JoinLobby.h"

#define STATUS 0

#define OK "ok"
#define ERROR "error"
JoinLobby::JoinLobby(MenuWindow &menu, LobbyWindow &lobby,
                     ServerMessenger& messenger) :
        menu(menu),
        lobby(lobby),
        messenger(messenger)
{
}

void JoinLobby::execute(const std::vector<std::string> &args) {
    if (args[STATUS] == OK) {
        menu.join_lobby();

        // Fetch available maps
        messenger.send("mapsinfo");
    }
}
