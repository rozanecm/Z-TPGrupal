#include "RemoveUnit.h"
#include <string>
#include <vector>
#define UNIT_ID 0

RemoveUnit::RemoveUnit(PlayersMonitor &monitor) : monitor(monitor) {
}

void RemoveUnit::execute(const std::vector<std::string> &args) {
    monitor.removeUnit(std::stoi(args[UNIT_ID]));
}
