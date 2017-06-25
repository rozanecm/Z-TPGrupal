#include "Loser.h"


Loser::Loser(MapMonitor& map, GameWindow &window) : map(map), window(window) {
}

void Loser::execute(const std::vector<std::string> &args) {
    map.finish_loser();
    window.hide();
}
