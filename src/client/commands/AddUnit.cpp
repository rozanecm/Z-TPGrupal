#include <iostream>
#include <vector>
#include "AddUnit.h"

#define UNIT_ID 0
#define X 1
#define Y 2
#define UNIT_TYPE 3
#define TEAM 4

const std::map<std::string, UnitsEnum> units = {
        {std::string("grunt"), UnitsEnum::GRUNT},
        {std::string("psycho"), UnitsEnum::PSYCHO},
        {std::string("tough"), UnitsEnum::TOUGH},
        {std::string("pyro"), UnitsEnum::PYRO},
        {std::string("sniper"), UnitsEnum::SNIPER},
        {std::string("laser"), UnitsEnum::LASER},
        {std::string("generic_robot"), UnitsEnum::GENERIC_ROBOT},
        {std::string("jeep"), UnitsEnum::JEEP},
        {std::string("medium_tank"), UnitsEnum::MEDIUM_TANK},
        {std::string("light_tank"), UnitsEnum::LIGHT_TANK},
        {std::string("heavy_tank"), UnitsEnum::HEAVY_TANK},
        {std::string("mml"), UnitsEnum::MML}
};

void AddUnit::execute(const std::vector<std::string> &args) {
    int x = std::stoi(args[X]);
    int y = std::stoi(args[Y]);
    int id = std::stoi(args[UNIT_ID]);
    auto type = units.find(args[UNIT_TYPE]);
    if (type == units.end()) {
        std::cerr << "Error adding unit: received type " << args[UNIT_TYPE] <<
                  std::endl;
        return;
    }

    Unit unit(id, {x, y}, type->second, (TeamEnum) std::stoi(args[TEAM]));
    unitsMonitor.addUnit(unit);
}

AddUnit::AddUnit(UnitsMonitor &unitsMonitor) : unitsMonitor(unitsMonitor) {
}
