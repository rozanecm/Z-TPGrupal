#include <iostream>
#include "UpdateUnit.h"

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

UpdateUnit::UpdateUnit(UnitsMonitor &units, MapMonitor& map,
                       ServerMessenger& messenger) :
        units(units),
        map(map),
        messenger(messenger)
{
}

void UpdateUnit::execute(const std::vector<std::string> &args) {
    int id = std::stoi(args[ID]);
    int x = std::stoi(args[X]);
    int y = std::stoi(args[Y]);
    ActionsEnum state = states.find(args[STATE])->second;
    unsigned int hp = std::stoul(args[HP]);
    units.update_position(id, state, x, y);
    units.update_health(id, hp);

    int flag = map.get_flag_at(x, y);
    if (flag > 0) {
        messenger.send("grab-" + std::to_string(id) + "-" + 
                               std::to_string(flag));
    }
}
