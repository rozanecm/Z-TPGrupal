#include "FactoryStats.h"
#include "../windows/GameWindow.h"

FactoryStats::FactoryStats(GameWindow& window) : window(window) {
}

#define TYPE 0
#define FIRE_RATE 1
#define HP 2

void FactoryStats::execute(const std::vector<std::string> &args) {
    std::string type = args[TYPE];
    int fire_rate = std::stoi(args[1]);
    int hp = std::stoi(args[2]);
    window.update_factory_panel(type, fire_rate, hp);
}
