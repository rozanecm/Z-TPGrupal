#include "StartGame.h"

StartGame::StartGame(ServerMessenger &m,
                     LobbyWindow& lobby,
                     GameWindow &window) :
        m(m),
        window(window),
        lobby(lobby) {
}

void StartGame::execute(const std::vector<std::string> &args) {
    lobby.start_game();
}

