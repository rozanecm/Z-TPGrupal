#include "UpdateFactory.h"

#define ID 0
#define TYPE 1
#define MINUTES 2
#define SECONDS 3
#define HP 4
#define TEAM 5

UpdateFactory::UpdateFactory(BuildingsMonitor &buildings) :
        buildings(buildings){
}

void UpdateFactory::execute(const std::vector<std::string> &args) {
    int id = std::stoi(args[ID]);
    std::string type = args[TYPE];
    int min = std::stoi(args[MINUTES]);
    int sec = std::stoi(args[SECONDS]);
    int hp = std::stoi(args[HP]);
    std::string team = args[TEAM];

    buildings.update_building(id, min, sec, hp);
}

