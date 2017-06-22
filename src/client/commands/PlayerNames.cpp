#include "PlayerNames.h"


PlayerNames::PlayerNames(LobbyWindow &lobby) : lobby(lobby) {
}

void PlayerNames::execute(const std::vector<std::string> &args) {
    for (int i = 0; i < args.size(); ++i) {
        lobby.update_player_name(i, args[i]);
    }
}