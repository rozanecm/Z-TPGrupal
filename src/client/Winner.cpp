#include "Winner.h"

Winner::Winner(bool &winner, GameWindow &window) :
        winner(winner), window(window) {
}

void Winner::execute(const std::vector<std::string> &args) {
    winner = true;
    window.hide();
}
