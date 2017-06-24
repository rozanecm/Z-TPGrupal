#include "Loser.h"


Loser::Loser(bool &loser, GameWindow &window) : loser(loser), window(window) {
}

void Loser::execute(const std::vector<std::string> &args) {
    loser = true;
    window.hide();
}
