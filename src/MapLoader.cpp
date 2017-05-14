#include "MapLoader.h"

#include "xml/pugixml.hpp"

using pugi::xml_attribute;

MapLoader::MapLoader(std::string path) {
    pugi::xml_document doc;
    doc.load_file(path.c_str());

    // Get root node
    pugi::xml_node_iterator map_node = doc.first_child();

    int coord_y = 0; // Y coordinate counter, each row is different Y coord
    // Iterate over every row
    auto row = map_node->children().begin();
    for (; row != map_node->children().end(); ++row) {
        int coord_x = 0; // Each Cell has a different X coord

        std::vector<Cell> row_vec;
        // Iterate over every row creating cells
        auto cell = row->children().begin();
        for (; cell != row->children().end(); ++cell) {
            std::string terrain = cell->attribute("terrain").value();
            std::string structure = cell->attribute("struct").value();
            int factor = 1; // TODO: actual factors

            // Create a real occupant if the cell's occupied
            Occupant o(-1);
            if (structure != "") {
                o = Occupant(std::stoi(structure));
            }

            // Create a new cell and push it to the row
            Cell c(coord_x, coord_y, terrain, factor, &o);
            row_vec.push_back(c);
            coord_x++;
        }

        // Push the whole row to the map
        map.push_back(row_vec);
        coord_y++;
    }
}


const std::vector<std::vector<Cell>>& MapLoader::get_map() {
    return map;
}

MapLoader::~MapLoader() {

}
