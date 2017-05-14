#ifndef Z_TPGRUPAL_MAPLOADER_H
#define Z_TPGRUPAL_MAPLOADER_H


#include <vector>
#include "cell.h"

class MapLoader {
    std::vector<std::vector<Cell>> map;
    std::vector<Occupant*> units;
public:
    MapLoader(std::string path);
    ~MapLoader();
    const std::vector<std::vector<Cell>>& get_map();

};


#endif //Z_TPGRUPAL_MAPLOADER_H
