#include "UpdatePosition.h"

#define UNIT_ID 0
#define POS_X 1
#define POS_Y 2

UpdatePosition::UpdatePosition(UnitsMonitor &monitor) : monitor(monitor) {
}

void UpdatePosition::execute(const std::vector<std::string> &args) {
    monitor.update_position(std::stoi(args[UNIT_ID]),
                            std::stoi(args[POS_X]),
                            std::stoi(args[POS_Y]));
}
