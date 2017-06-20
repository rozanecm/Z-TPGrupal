#include "AddBuilding.h"

#define TILESIZE 16    //tile width in pixels.
#define ID 0
#define X 1
#define Y 2
#define TYPE 3
#define TEAM 4

AddBuilding::AddBuilding(BuildingsMonitor &buildings) : buildings(buildings) {
}

void AddBuilding::execute(const std::vector<std::string> &args) {
    int id = std::stoi(args[ID]);
    int x = std::stoi(args[X]);
    int y = std::stoi(args[Y]);
    TeamEnum team = (TeamEnum) std::stoi(args[TEAM]);

    Building b(BuildingsEnum::ROBOT_FABRIC, x, y, id, team);
    buildings.addBuilding(b);
}
