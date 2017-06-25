#include <map>
#include "UpdateTerritory.h"
#include "../enums/TeamEnum.h"

#define ID 0
#define TEAM 1
#define X 2
#define Y 3

const std::map<std::string, TeamEnum> teams = {
        {std::string("blue"), TeamEnum::BLUE},
        {std::string("green"), TeamEnum::GREEN},
        {std::string("neutral"), TeamEnum::NEUTRAL},
        {std::string("red"), TeamEnum::RED},
        {std::string("yellow"), TeamEnum::YELLOW},
};

UpdateTerritory::UpdateTerritory(MapMonitor &mapMonitor1)
        : mapMonitor(mapMonitor1){
}

void UpdateTerritory::execute(const std::vector<std::string> &args) {
    int id = std::stoi(args[ID]);
    TeamEnum team = (TeamEnum) mapMonitor.get_player_id(args[TEAM]);
    teams.find(args[TEAM])->second;
    int x = std::stoi(args[X]);
    int y = std::stoi(args[Y]);

    mapMonitor.update_territory(id, team, x, y);

//    updateterritory-[id]-[team]-[x]-[y]
}
