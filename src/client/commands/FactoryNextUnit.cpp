#include "FactoryNextUnit.h"

#define UNIT_NAME 0

FactoryNextUnit::FactoryNextUnit(GameWindow &window) : window(window) {
}

void FactoryNextUnit::execute(const std::vector<std::string> &args) {
    std::string path = "res/portraits/" + args[UNIT_NAME] + ".png";
    window.factory_change_unit(path);
}


