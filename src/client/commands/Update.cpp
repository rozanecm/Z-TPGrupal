#include <iostream>
#include "Update.h"

#define ID 0
#define STATE 1
#define X 2
#define Y 3
#define HP 4

Update::Update(UnitsMonitor& units) : units(units){
}

void Update::execute(const std::vector<std::string> &args) {
    int id = std::stoi(args[ID]);
    int x = std::stoi(args[X]);
    int y = std::stoi(args[Y]);
    units.update_position(id, x, y);
}
