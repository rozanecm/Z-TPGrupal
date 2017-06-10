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

MapLoader::MapLoader(std::string path) {
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

    int id_counter = 0;
    pugi::xml_node structs = root.child("Structures");
    auto rock = structs.children().begin();
    // Todo: read values from cfg file
    for(; rock != structs.children().end(); ++rock) {
        int x = std::stoi(rock->attribute("x").value());
        int y = std::stoi(rock->attribute("y").value());
        std::string type = rock->attribute("Type").value();
        occupants.emplace_back(id_counter++, 10, type, Size(x, y, 3, 3));
    }

    pugi::xml_node territories = root.child("Territories");
    auto territory = territories.children().begin();
    for(; territory != territories.children().end(); ++territory) {
        auto factory = territory->children().begin();
        for(; factory != territory->children().end(); ++factory) {
            int x = std::stoi(factory->attribute("x").value());
            int y = std::stoi(factory->attribute("y").value());
            std::string type = factory->name();
            occupants.emplace_back(id_counter++, 1000, type, Size(x, y, 9, 9));
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
