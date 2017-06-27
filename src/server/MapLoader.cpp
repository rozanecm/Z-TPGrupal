#include <map>
#include "MapLoader.h"
#include <pugixml.hpp>
#include <sstream>


const std::map<std::string, int> terrain_factor {
        {std::string("Tierra"), int(1)},
        {std::string("Agua"), int(7/10)},
        {std::string("Carretera"), int(15/10)},
        {std::string("Lava"), int(1000)}
};

MapLoader::MapLoader(std::string path, std::string& config) : config(config) {
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(path.c_str());
    if (!result) {
        std::cout << "ERROR LOADING MAP: " << path << ": " <<
                  result.description() << std::endl;
    }
    std::stringstream stream;
    doc.save(stream);
    map_string = stream.str();
    // Get root node
    pugi::xml_node root = doc.child("Map");
    pugi::xml_node map_node = root.child("Terrain");

    pugi::xml_document cfg;
    cfg.load_file(config.c_str());
    pugi::xml_node cfg_node = cfg.child("Config");

    internal_positions = std::stoi(cfg_node.child("Cells").
            attribute("internal_positions").value());

    // Iterate over every row
    unsigned int coord_y = 0;
    auto row = map_node.children().begin();
    for (; row != map_node.children().end(); ++row) {
        unsigned int coord_x = 0;
        // Iterate over every row creating cells
        auto cell = row->children().begin();
        for (; cell != row->children().end(); ++cell) {
            if (map.size() <= (coord_x)) {
                map.push_back(std::vector<Cell>());
            }

            std::string terrain = cell->attribute("terrain").value();
            std::string structure = cell->attribute("struct").value();

            int factor = terrain_factor.find(terrain)->second;

            // Create a new cell and push it to the row
            map.at(coord_x).emplace_back(coord_x * internal_positions, coord_y,
                                         internal_positions, internal_positions,
                                         terrain, factor);
            coord_x++;
        }
        // Push the whole row to the map
        coord_y += internal_positions;
    }



    create_map();
    load_unit_molds(cfg_node.child("Units"));
    load_vehicle_molds(cfg_node.child("Vehicles"));
    load_weapons(cfg_node.child("Weapons"));
    load_structs(root, cfg_node.child("Structs"));
}

void MapLoader::load_structs(const pugi::xml_node &root,
                             const pugi::xml_node &cfg) {

    int id_counter = 0;
    pugi::xml_node structs = root.child("Structures");
    pugi::xml_node structure_cfg = cfg.find_child_by_attribute("type", "Rock");
    int size_x = std::stoi(structure_cfg.attribute("size_x").value());
    int size_y = std::stoi(structure_cfg.attribute("size_y").value());
    int hp = std::stoi(structure_cfg.attribute("hp").value());

    std::string type = structure_cfg.attribute("type").value();
    for(auto& rock : structs) {
        int x = std::stoi(rock.attribute("x").value()) *
            internal_positions;
        int y = std::stoi(rock.attribute("y").value()) *
            internal_positions;
        Occupant* f = new Occupant(id_counter++, hp, type,
                                   Size(x, y, size_x, size_y));
        occupants.push_back(f);
    }

    load_territories(cfg, root.child("Territories"), id_counter);

}

void MapLoader::create_map() {
    int width = (int) map.at(0).size() * internal_positions;
    int height = (int) map.size() * internal_positions;
    int x = 0;
    int y = 0;
    game_map = std::shared_ptr<Map>(new Map(x, y, width, height, map,
                                            &occupants, map_string));
}

MapLoader::~MapLoader() {
}

std::vector<Occupant*> MapLoader::getOccupants() {
    return this->occupants;
}

std::vector<Unit> MapLoader::getUnits() {
    return this->units;
}

Factory *MapLoader::create_factory(int id, int hp, std::string &type, Size size) {
    std::vector<UnitMold*> both = unit_mold;
    for (UnitMold* vehicle : vehicle_mold) {
        both.push_back(vehicle);
    }
    return (new Factory(id, hp, type, size, both, game_map, weapons));
}

void MapLoader::load_unit_molds(pugi::xml_node units) {
    load_mold(unit_mold, units);
}

void MapLoader::load_weapons(pugi::xml_node weapons) {
    for(auto& unit : weapons.children()) {
        std::string type = unit.attribute("type").value();
        int width = std::stoi(unit.attribute("size_x").value());
        int height = std::stoi(unit.attribute("size_y").value());
        int speed = std::stoi(unit.attribute("speed").value());
        int damage = std::stoi(unit.attribute("damage").value());
        std::string exp = unit.attribute("explosive").value();
        bool explosive = false;
        if (exp == "yes")
            explosive = true;

        Size s(0, 0, width, height);
        this->weapons.emplace(type, Weapon(type, damage, speed, explosive, s));
    }
}

std::shared_ptr<Map> MapLoader::get_map() {
    return game_map;
}

void MapLoader::load_territories(const pugi::xml_node &structs_cfg,
                                 const pugi::xml_node &root, int id_counter) {
    pugi::xml_node factory_cfg = structs_cfg.
            find_child_by_attribute("type", "Factory");

    int size_x = std::stoi(factory_cfg.attribute("size_x").value());
    int size_y = std::stoi(factory_cfg.attribute("size_y").value());
    int hp = std::stoi(factory_cfg.attribute("hp").value());
    std::string type = factory_cfg.attribute("type").value();

    for(auto& territory : root.children()) {
        std::map<int, Factory*> factories_in_territory;
        for(auto& factory : territory.children()) {
            std::vector<UnitMold*> mold = unit_mold;
            std::string name = factory.name();
            if (name == "VehicleFactory") {
                mold = vehicle_mold;
            }

            int x = std::stoi(factory.attribute("x").value()) *
                internal_positions;
            int y = std::stoi(factory.attribute("y").value()) *
                internal_positions;
            Size s(x, y, size_x, size_y);
            Factory* f = new Factory(id_counter, hp, type, s, mold, game_map, weapons);
            factories_in_territory[id_counter] = f;
            ++id_counter;
        }
        create_territory(hp, territory, id_counter, factories_in_territory);
    }
}

void MapLoader::create_territory(int hp, const pugi::xml_node &territory,
                                 int &id_counter,
                                 std::map<int, Factory *> &factories_in_territory) {
    std::string name = territory.name();
    int x = std::stoi(territory.attribute("center_x").value()) *
            internal_positions;
    int y = std::stoi(territory.attribute("center_y").value()) *
            internal_positions;
    /* If it's a fort we also create a factory */
    if (name == "Fort") {
        Factory* f = create_factory(id_counter, hp, name,
                                    Size(x, y, 20, 20));
        factories_in_territory[id_counter] = f;
        forts.push_back(f);
        ++id_counter;
    }

    int min_x = std::stoi(territory.attribute("min_x").value());
    int min_y = std::stoi(territory.attribute("min_y").value());
    int max_x = std::stoi(territory.attribute("max_x").value());
    int max_y = std::stoi(territory.attribute("max_y").value());
    int width =  (min_x + max_x) / 2;
    int height = (min_y + max_y) / 2;
    Size flag(x, y, width, height);
    Position flag_position(x, y);
    Territory* t = new Territory(factories_in_territory, flag_position, flag, 
    ++id_counter);
    territories.emplace_back(t);
}

std::vector<Factory *> MapLoader::get_forts() {
    return forts;
}

std::vector<Territory *> MapLoader::get_territories() {
    return territories;
}

void MapLoader::load_vehicle_molds(const pugi::xml_node &vehicles) {
    load_mold(vehicle_mold, vehicles);
}

void MapLoader::load_mold(std::vector<UnitMold *>& mold,
                          const pugi::xml_node &source) {
    for (auto& unit : source.children()) {
        std::string type = unit.attribute("type").value();
        std::string weapon = unit.attribute("weapon").value();
        int width = std::stoi(unit.attribute("size_x").value());
        int height = std::stoi(unit.attribute("size_y").value());
        int hp = std::stoi(unit.attribute("hp").value());
        int fire_rate = std::stoi(unit.attribute("fire_rate").value());
        int range = std::stoi(unit.attribute("range").value());
        int speed = std::stoi(unit.attribute("speed").value());
        int time = std::stoi(unit.attribute("time").value());
        int quantity = std::stoi(unit.attribute("quantity").value());
        int tech_level = std::stoi(unit.attribute("tech_level").value());
        mold.push_back(
                new UnitMold(tech_level, hp, range, width, height, speed,
                             fire_rate, time, quantity, type, weapon));
    }
}



