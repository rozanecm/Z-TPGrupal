#ifndef Z_TPGRUPAL_MAPLOADER_H
#define Z_TPGRUPAL_MAPLOADER_H


#include <vector>
#include <pugixml.hpp>
#include <memory>
#include "map.h"
#include "Occupant.h"
#include "cell.h"
#include "unit.h"
#include "factory.h"
#include "territory.h"

class MapLoader {
    std::vector<std::vector<Cell>> map;
    std::vector<Occupant *> occupants;
    std::vector<Unit> units;
    std::string map_string;
    std::string &config;
    std::vector<UnitMold> unit_mold;
    std::map<std::string, Weapon> weapons;
    std::shared_ptr<Map> game_map;
    std::vector<Factory*> forts;
    std::vector<Territory*> territories;
    int internal_positions;
public:
    MapLoader(std::string path, std::string& config);

    ~MapLoader();


    std::vector<Occupant*> getOccupants();

    std::vector<Unit> getUnits();

    std::shared_ptr<Map> get_map();
    std::vector<Factory*> get_forts();
private:
    void load_structs(const pugi::xml_node &root, const pugi::xml_node &cfg);

    Factory* create_factory(int id, int hp, std::string& type, Size size);

    void load_unit_molds(pugi::xml_node node);
    void load_weapons(pugi::xml_node weapons);
    void create_map();
    void load_factories(const pugi::xml_node &structs_cfg,
                        const pugi::xml_node &root, int id_counter);

    void create_territory(int hp, const pugi::xml_node &territory, 
                          int &id_counter,
                          std::map<int, Factory *> &factories_in_territory);
    
    std::vector<Territory*> get_territories();
};


#endif //Z_TPGRUPAL_MAPLOADER_H
