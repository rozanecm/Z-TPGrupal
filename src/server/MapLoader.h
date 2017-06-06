#ifndef Z_TPGRUPAL_MAPLOADER_H
#define Z_TPGRUPAL_MAPLOADER_H


#include <vector>
#include "map.h"
#include "Occupant.h"
#include "cell.h"
#include "unit.h"

class MapLoader {
    std::vector<std::vector<Cell>> map;
    std::vector<Occupant> occupants;
    std::vector<Unit> units;
public:
    MapLoader(std::string path);

    ~MapLoader();

    Map get_map();

    std::vector<Occupant> getOccupants();

    std::vector<Unit> getUnits();
};


#endif //Z_TPGRUPAL_MAPLOADER_H
