#include "AddBuilding.h"

#define TILESIZE 16    //tile width in pixels.
#define ID 0
#define X 1
#define Y 2
#define TYPE 3
#define TEAM 4
#define HP 5

const std::map<std::string, BuildingsEnum> buildingsMap = {
        {std::string("Fort"), BuildingsEnum::FORT},
        /* Since we dont yet distinguish between robot and vehicle
         * factories, all factories will be drawn as robot factories. */
        {std::string("Factory"), BuildingsEnum::ROBOT_FABRIC}
};

AddBuilding::AddBuilding(BuildingsMonitor &buildings,
                         MapMonitor& map)
        : buildings(buildings),
          map(map)
{
}

void AddBuilding::execute(const std::vector<std::string> &args) {
    int id = std::stoi(args[ID]);
    int x = std::stoi(args[X]);
    int y = std::stoi(args[Y]);

    BuildingsEnum building_type = buildingsMap.find(args[TYPE])->second;

    std::string owner = args[TEAM];
    int team_id = map.get_player_id(owner);

    unsigned int hp = std::stoul(args[HP]);
    Building b(building_type, x, y, id, (TeamEnum) team_id, owner, hp);
    buildings.addBuilding(b);
}
