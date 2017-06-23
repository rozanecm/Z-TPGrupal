#include "UpdateFactory.h"

#define ID 0
#define MINUTES 1
#define SECONDS 2
#define HP 3
#define TEAM 4

UpdateFactory::UpdateFactory(BuildingsMonitor &buildings) :
        buildings(buildings){
}

void UpdateFactory::execute(const std::vector<std::string> &args) {
    int id = std::stoi(args[ID]);
    int min = std::stoi(args[MINUTES]);
    int sec = std::stoi(args[SECONDS]);
    int hp = std::stoi(args[HP]);
    std::string team = args[TEAM];

    buildings.update_building(id, min, sec, hp);
}

