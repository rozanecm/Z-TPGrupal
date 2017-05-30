#include <iostream>
#include <string>
#include <vector>
#include "AddUnit.h"

#define PLAYER_ID 0
#define UNIT_ID 1
void AddUnit::execute(const std::vector<std::string> &args) {
    for (const std::string& arg : args) {
        std::cout << arg << std::endl;
    }
    Unit unit;
    playerMonitor.addUnit(std::stoi(args[PLAYER_ID]), unit);
}

AddUnit::AddUnit(PlayersMonitor &monitor) : playerMonitor(monitor) {
}
