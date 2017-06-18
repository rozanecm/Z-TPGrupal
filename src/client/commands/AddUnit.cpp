#include <iostream>
#include <vector>
#include "AddUnit.h"

#define UNIT_ID 0
#define X 1
#define Y 2
#define UNIT_TYPE 3
const std::map<std::string, UnitsEnum> units = {
        {std::string("grunt"), UnitsEnum::GRUNT}
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

    Unit unit(id, {x, y}, type->second, RotationsEnum::r000);
    unitsMonitor.addUnit(unit);
}

AddUnit::AddUnit(UnitsMonitor &unitsMonitor) : unitsMonitor(unitsMonitor) {
}
