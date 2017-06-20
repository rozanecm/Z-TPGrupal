#include <iostream>
#include "Update.h"

#define ID 0
#define STATE 1
#define X 2
#define Y 3
#define HP 4

const std::map<std::string, ActionsEnum> states = {
        {std::string("atk"), ActionsEnum::FIRE},
        {std::string("mv"), ActionsEnum::MOVE},
        {std::string("std"), ActionsEnum::STAND}
};

Update::Update(UnitsMonitor& units) : units(units){
}

void Update::execute(const std::vector<std::string> &args) {
    int id = std::stoi(args[ID]);
    int x = std::stoi(args[X]);
    int y = std::stoi(args[Y]);
    ActionsEnum state = states.find(args[STATE])->second;
    units.update_position(id, state, x, y);
}
