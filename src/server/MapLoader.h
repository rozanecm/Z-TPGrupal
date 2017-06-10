#ifndef Z_TPGRUPAL_MAPLOADER_H
#define Z_TPGRUPAL_MAPLOADER_H


#include <vector>
#include <pugixml.hpp>
#include "map.h"
#include "Occupant.h"
#include "cell.h"
#include "unit.h"

class MapLoader {
    std::vector<std::vector<Cell>> map;
    std::vector<Occupant> occupants;
    std::vector<Unit> units;
    std::string map_string;
    std::string& config;
public:
    MapLoader(std::string path, std::string& config);

    ~MapLoader();

    Map get_map();

    std::vector<Occupant> getOccupants();

    std::vector<Unit> getUnits();

private:
    void load_structs(const pugi::xml_node &root);

    void load_struct(const pugi::xml_node &structs_cfg, int id_counter,
               const pugi::xml_node_iterator &structure);
};


#endif //Z_TPGRUPAL_MAPLOADER_H
