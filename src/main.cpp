#include <gtkmm.h>
#include <iostream>
#include "GameWindow.h"
#include "cell.h"
#include "xml/pugixml.hpp"

using pugi::xml_attribute;
std::vector<std::vector<Cell>> load_map() {
    std::vector<std::vector<Cell>> map;
    pugi::xml_document doc;
    doc.load_file("src/map.xml");

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
    return map;
}

int main (int argc, char **argv)
{
    std::vector<std::vector<Cell>> map = load_map();

    for(auto it = map.begin(); it != map.end(); ++it) {
        for(auto jt = it->begin(); jt != it->end(); ++jt) {
            std::cout << jt->isEmpty() << std::endl;
        }
    }

    auto app = Gtk::Application::create(argc, argv);

    GameWindow game_window;
    Gtk::Window* window = game_window.get_window();
    if (window)
    {
        return app->run(*window);
    }

    return 1;
}

