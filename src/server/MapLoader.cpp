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

    int coord_y = 1; // Y coordinate counter, each row is different Y coord
    // Iterate over every row
    auto row = map_node.children().begin();
    for (; row != map_node.children().end(); ++row) {
        int coord_x = 1; // Each Cell has a different X coord

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
            map.back().emplace_back(coord_y, coord_x, 3, 3, terrain, factor);
            coord_x += 3;
        }

        // Push the whole row to the map
        coord_y += 3;
    }

    load_structs(root);
}

void MapLoader::load_structs(const pugi::xml_node &root) {
    pugi::xml_document cfg;
    cfg.load_file(config.c_str());
    pugi::xml_node structs_cfg = cfg.child("Config").child("Structs");

    int id_counter = 0;
    pugi::xml_node structs = root.child("Structures");
    auto rock = structs.children().begin();

    for(; rock != structs.children().end(); ++rock) {
        load_struct(structs_cfg, id_counter++, rock);
    }

    pugi::xml_node territories = root.child("Territories");
    for(auto& territory : territories.children()) {
        for(auto& factory : territory.children()) {
            load_struct(structs_cfg, id_counter++, factory);
        }
    }
}

void MapLoader::load_struct(const pugi::xml_node &structs_cfg, int id_counter,
                      const pugi::xml_node_iterator &structure) {
    int x = std::stoi(structure->attribute("x").value());
    int y = std::stoi(structure->attribute("y").value());
    std::string type = structure->attribute("Type").value();

    /* Search the config file for the appropiate struct row to read
     * the struct's stats from */
    for(auto& child : structs_cfg.children()) {
        if (child.attribute("type").value() == type) {
            int hp = std::stoi(child.attribute("hp").value());
            int size_x = std::stoi(child.attribute("size_x").value());
            int size_y = std::stoi(child.attribute("size_y").value());
            occupants.emplace_back(id_counter, hp, type,
                                   Size(x, y, size_x, size_y));
            break;
        }
    }
}


Map MapLoader::get_map() {
    int width = (int) map.at(0).size();
    int height = (int) map.size();
    int x = width / 2;
    int y = height / 2;
    return Map(x, y, width, height, map, occupants, map_string);
}

MapLoader::~MapLoader() {
}

std::vector<Occupant> MapLoader::getOccupants() {
    return this->occupants;
}

std::vector<Unit> MapLoader::getUnits() {
    return this->units;
}
