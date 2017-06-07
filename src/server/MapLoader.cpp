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
    const std::map<std::string, int> terrain_factor;
    // Get root node
    pugi::xml_node_iterator map_node = doc.first_child();

    int coord_y = 1; // Y coordinate counter, each row is different Y coord
    // Iterate over every row
    auto row = map_node->children().begin();
    for (; row != map_node->children().end(); ++row) {
        int coord_x = 1; // Each Cell has a different X coord

        std::vector<Cell> row_vec;
        map.push_back(row_vec);

        // Iterate over every row creating cells
        auto cell = row->children().begin();
        for (; cell != row->children().end(); ++cell) {
            std::string terrain = cell->attribute("terrain").value();
            std::string structure = cell->attribute("struct").value();



//            auto it = terrain_factor.find(terrain); // TODO: actual factors
            double factor = 1;
            if (terrain == "Agua") {
                factor = 0.7;
            } else if (terrain == "Lava") {
                factor = 1000;
            } else if (terrain == "Carretera") {
                factor = 1.5;
            }

            // Create a real occupant if the cell's occupied
            int ocpt_id = -1;
            if (structure != "") {
                ocpt_id = std::stoi(structure);
            }
            ////
//            int life = 100;
//            Size size(10,10, 10, 10);
//            ////
//            Occupant* o = new Occupant(life, ocpt_id, size);
//            units.push_back(o);

            // Create a new cell and push it to the row
            // first 10 is for width, the other is for height
            map.back().emplace_back(coord_y, coord_x, 3, 3, terrain, factor);
            coord_x += 3;
        }

        // Push the whole row to the map
        coord_y += 3;
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
