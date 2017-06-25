#include "Winner.h"

Winner::Winner(MapMonitor& map, GameWindow &window) :
        map(map), window(window) {
}

void Winner::execute(const std::vector<std::string> &args) {
    map.finish_winner();
    window.hide();
}
