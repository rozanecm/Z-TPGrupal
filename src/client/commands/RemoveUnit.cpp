#include "RemoveUnit.h"
#include "../UnitsMonitor.h"
#include <string>
#include <vector>
#define UNIT_ID 0

RemoveUnit::RemoveUnit(UnitsMonitor &monitor) : monitor(monitor) {
}

void RemoveUnit::execute(const std::vector<std::string> &args) {
    monitor.removeUnit(std::stoi(args[UNIT_ID]));
}
