#include "FactoryStats.h"
#include "../windows/GameWindow.h"

FactoryStats::FactoryStats(GameWindow& window) : window(window) {
}

#define TYPE 0
#define FIRE_RATE 1
#define MINUTES 2
#define SECONDS 3
#define HP 4

void FactoryStats::execute(const std::vector<std::string> &args) {
    std::string type = args[TYPE];
    int fire_rate = std::stoi(args[FIRE_RATE]);
    int hp = std::stoi(args[HP]);
    int minutes = std::stoi(args[MINUTES]);
    int seconds = std::stoi(args[SECONDS]);
    window.update_factory_panel(type, fire_rate, hp);
    window.update_factory_timer(minutes, seconds);
}
