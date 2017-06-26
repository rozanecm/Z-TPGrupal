#include "UpdatePosition.h"
#include "../MapMonitor.h"
#include "../ServerMessenger.h"

#define UNIT_ID 0
#define POS_X 1
#define POS_Y 2

UpdatePosition::UpdatePosition(UnitsMonitor &monitor) :
        units(monitor)
{
}

void UpdatePosition::execute(const std::vector<std::string> &args) {
    int id = std::stoi(args[UNIT_ID]);
    int x = std::stoi(args[POS_X]);
    int y = std::stoi(args[POS_Y]);
    units.update_position(id, x, y);
}
