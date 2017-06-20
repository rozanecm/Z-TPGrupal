#include <map>
#include "AddNature.h"

#define ID 0
#define X 1
#define Y 2
#define TYPE 3
#define TEAM 4

const std::map<std::string, NatureEnum > natureMap = {
        {std::string("Rock"), NatureEnum ::ROCK}
};


AddNature::AddNature(MapMonitor &mapMonitor) : mapMonitor(mapMonitor){
}

void AddNature::execute(const std::vector<std::string> &args) {
    int id = std::stoi(args[ID]);
    int x = std::stoi(args[X]);
    int y = std::stoi(args[Y]);
    NatureEnum natureType = natureMap.find(args[TYPE])->second;

    Nature nature(natureType, {x, y}, id);

    mapMonitor.addNature(nature);
//    Building b(BuildingsEnum::ROBOT_FABRIC, x, y, id, team);
//    buildings.addBuilding(b);
}
