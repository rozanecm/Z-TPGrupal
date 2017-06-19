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

    int coord_y = 8; // Y coordinate counter, each row is different Y coord
    // Iterate over every row
    auto row = map_node.children().begin();
    for (; row != map_node.children().end(); ++row) {
        int coord_x = 8; // Each Cell has a different X coord

        std::vector<Cell> row_vec;
        map.push_back(row_vec);
        // Iterate over every row creating cells
        auto cell = row->children().begin();
        for (; cell != row->children().end(); ++cell) {
            std::string terrain = cell->attribute("terrain").value();
            std::string structure = cell->attribute("struct").value();

            int factor = terrain_factor.find(terrain)->second;
            
            // Create a new cell and push it to the row
            // first 10 is for width, the other is for height
            map.back().emplace_back(coord_y, coord_x, 16, 16, terrain, factor);
            coord_x += 16;
        }
        // Push the whole row to the map
        coord_y += 16;
    }

    pugi::xml_document cfg;
    cfg.load_file(config.c_str());
    pugi::xml_node cfg_node = cfg.child("Config");
    create_map();
    load_structs(root, cfg_node.child("Structs"));
    load_unit_molds(cfg_node.child("Units"));
    load_weapons(cfg_node.child("Weapons"));
}

void MapLoader::load_structs(const pugi::xml_node &root,
                             const pugi::xml_node &cfg) {

    int id_counter = 0;
    pugi::xml_node structs = root.child("Structures");
    pugi::xml_node structure_cfg = cfg.find_child_by_attribute("type", "Rock");
    int x = std::stoi(structure_cfg.attribute("size_x").value());
    int y = std::stoi(structure_cfg.attribute("size_y").value());
    int hp = std::stoi(structure_cfg.attribute("hp").value());

    std::string type = structure_cfg.attribute("type").value();
    for(auto& rock : structs) {
        int size_x = std::stoi(rock.attribute("x").value());
        int size_y = std::stoi(rock.attribute("y").value());
        Occupant* f = new Occupant(id_counter++, hp, type,
                                   Size(x, y, size_x, size_y));
        occupants.push_back(f);
    }

    load_factories(cfg, root.child("Territories"), id_counter);

}

void MapLoader::create_map() {
    int width = (int) map.at(0).size() * 16;
    int height = (int) map.size() * 16;
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

    return new Factory(id, hp, type, size, unit_mold, game_map, weapons);
}

void MapLoader::load_unit_molds(pugi::xml_node units) {
    for (auto& unit : units.children()) {
        std::string type = unit.attribute("type").value();
        int width = std::stoi(unit.attribute("size_x").value());
        int height = std::stoi(unit.attribute("size_y").value());
        int hp = std::stoi(unit.attribute("hp").value());
        int fire_rate = std::stoi(unit.attribute("fire_rate").value());
        int range = std::stoi(unit.attribute("range").value());
        int speed = std::stoi(unit.attribute("speed").value());
        int time = std::stoi(unit.attribute("time").value());
        int tech_level = std::stoi(unit.attribute("tech_level").value());
        unit_mold.emplace_back(tech_level, hp, range, width, height,
                speed, fire_rate, time, type);
    }
}

void MapLoader::load_weapons(pugi::xml_node weapons) {
    for(auto& unit : weapons.children()) {
        std::string type = unit.attribute("type").value();
        int width = std::stoi(unit.attribute("size_x").value());
        int height = std::stoi(unit.attribute("size_y").value());
        int speed = std::stoi(unit.attribute("speed").value());
        int damage = std::stoi(unit.attribute("damage").value());
        bool explosive = type == "yes";

        Size s(0, 0, width, height);
        this->weapons.emplace(type, Weapon(type, damage, speed, explosive, s));
    }
}

std::shared_ptr<Map> MapLoader::get_map() {
    return game_map;
}

void MapLoader::load_factories(const pugi::xml_node &structs_cfg,
                               const pugi::xml_node &root, int id_counter) {
    pugi::xml_node factory_cfg = structs_cfg.
            find_child_by_attribute("type", "Factory");

    int size_x = std::stoi(factory_cfg.attribute("size_x").value());
    int size_y = std::stoi(factory_cfg.attribute("size_y").value());
    int hp = std::stoi(factory_cfg.attribute("hp").value());
    std::string type = factory_cfg.attribute("type").value();
    for(auto& territory : root.children()) {
        std::string name = territory.name();
        if (name == "Fort") {
            int x = std::stoi(territory.attribute("center_x").value());
            int y = std::stoi(territory.attribute("center_y").value());
            forts.push_back(create_factory(id_counter++, hp, name,
                                           Size(x, y, 20, 20)));
        }
        for(auto& factory : territory.children()) {
            int x = std::stoi(factory.attribute("x").value());
            int y = std::stoi(factory.attribute("y").value());
            Size s(x, y, size_x, size_y);
            occupants.push_back(create_factory(id_counter++, hp, type, s));
        }
    }
}

std::vector<Factory *> MapLoader::get_forts() {
    return forts;
}


