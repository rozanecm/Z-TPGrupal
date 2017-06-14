#include <iostream>
#include <string>
#include <vector>
#include "AddUnit.h"
#include "../enums/UnitsEnum.h"
#include "../enums/RotationsEnum.h"
#include "../UnitsMonitor.h"

#define PLAYER_ID 0
#define UNIT_ID 1
void AddUnit::execute(const std::vector<std::string> &args) {
    for (const std::string& arg : args) {
        std::cout << arg << std::endl;
    }
    int x = std::stoi(args[0]);
    int y = std::stoi(args[1]);
    Unit unit(std::stoi(args[PLAYER_ID]), std::pair<unsigned int, unsigned
    int>(), UnitsEnum::HEAVY_TANK, RotationsEnum::r000);
    unitsMonitor.addUnit(unit);
}

AddUnit::AddUnit(UnitsMonitor &unitsMonitor) : unitsMonitor(unitsMonitor) {
}
