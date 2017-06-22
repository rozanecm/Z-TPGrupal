#include <iostream>
#include <vector>
#include "AddUnit.h"

#define UNIT_ID 0
#define X 1
#define Y 2
#define UNIT_NAME 3
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
    std::string name = args[UNIT_NAME];
    auto type = units.find(name);
    if (type == units.end()) {
        std::cerr << "Error adding unit: received type " << args[UNIT_NAME] <<
                  std::endl;
        return;
    }

    std::string owner = args[TEAM];
    int team_id = 1;
    for (const std::string& player : players) {
        if (player == owner) {
            break;
        }
        team_id++;
    }
    if (team_id > players.size()) {
        team_id = 0; // set it to team NEUTRAL
    }
    Unit unit(id, {x, y}, type->second, (TeamEnum) team_id);
    unit.update_owner(owner);
    unit.update_unit_name(name);
    unitsMonitor.addUnit(unit);
}

AddUnit::AddUnit(UnitsMonitor &unitsMonitor,
                 const std::vector<std::string>& players)
        : unitsMonitor(unitsMonitor),
          players(players)
{
}
