#include <fstream>
#include <vector>
#include <iostream>
#include "MapGenerator.h"
#include <pugixml.hpp>

#define RIVER_END_PCT 5
#define TERRAIN "terrain"

using namespace pugi;

void print_map(xml_node& root_node) {
    for(xml_node& row : root_node.children()) {
        for (xml_node &node : row.children()) {
            char terrain = node.attribute(TERRAIN).value()[0];
            if (terrain == 'T') {
                terrain = ' ';
            }
            std::cout << terrain << " ";
        }
        std::cout << std::endl;
    }
}


MapGenerator::MapGenerator(int size, float lava_pct, float water_pct,
                           float terrain_variance_pct) :
    size(size),
    lava_pct(lava_pct),
    water_pct(water_pct),
    terrain_var(terrain_variance_pct)
{
    for (int i = 0; i < size; ++i) {
        std::vector<bool> row;
        for (int j = 0; j < size; ++j) {
            row.push_back(false);
        }
        occupied_cells.push_back(row);
    }
    water_cells = (int) (size * size * water_pct / 100);
    lava_cells = (int) (size * size * lava_pct / 100);
}

void MapGenerator::generate_blank_map(xml_node root_node) {
    for (int i = 0; i < size; ++i) {
        xml_node row = root_node.append_child("Row");
        for (int j = 0; j < size; ++j) {
            xml_node cell = row.append_child("Cell");
            xml_attribute attr = cell.append_attribute(TERRAIN);
            attr.set_value("Tierra");
        }
    }
}

std::vector<std::vector<bool>> MapGenerator::generate_rivers(pugi::xml_node root_node, int cell_amt,
                                   const std::string &terrain) {
    std::vector<std::vector<bool>> map;
    generate_path(cell_amt, time(NULL), map);
    int count_y = 0;
    for(xml_node& row : root_node.children()) {
        int count_x = 0;
        for(xml_node& node : row.children()) {
            if (map[count_x][count_y]) {
                node.attribute(TERRAIN).set_value(terrain.c_str());
            }
            count_x++;
        }
        count_y++;
    }

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; ++j) {
            if (map[i][j]) {
                occupied_cells[i][j] = true;
            }
        }
    }
    return map;
}

void MapGenerator::generate_path(int amt, time_t seed,
                                 std::vector<std::vector<bool>>& path) {
    srand((unsigned int) seed);
    for (int i = 0; i < size; ++i) {
        std::vector<bool> row;
        for (int j = 0; j < size; ++j) {
            row.push_back(false);
        }
        path.push_back(row);
    }

    int river_x = rand() % size;
    int river_y = rand() % size;

    while (amt) {
        path[river_x][river_y] = true;

        bool found = false;

        while (!found) {
            int end = rand() % 100;
            if (end < RIVER_END_PCT) { // Start another river somewhere else
                river_x = rand() % size;
                river_y = rand() % size;
            }
            // Grab an adjacent tile randomly to be the next water tile
            int next = rand() % 4;
            int next_x, next_y;
            if (next == 0) {
                next_x = 1;
                next_y = 0;
            } else if (next == 1) {
                next_x = 0;
                next_y = -1;
            } else if (next == 2) {
                next_x = -1;
                next_y = 0;
            } else {
                next_x = 0;
                next_y = 1;
            }
            next_x += river_x;
            next_y += river_y;


            // Check for out of bounds
            if (!(next_x > 0 && next_y > 0 && next_x < size && next_y < size)) {
                continue;
            }

            if (!path[next_x][next_y]) {
                found = true;
                amt--;
                river_x = next_x;
                river_y = next_y;
            }
        }
    }
}

void MapGenerator::generate_structs(xml_node root) {
    root.set_name("Structures");
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            if (!occupied_cells[i][j]) {
                int chance = rand() % 100;
                if (chance < ROCK_PCT) {
                    xml_node rock = root.append_child("Struct");
                    rock.append_attribute("Type").set_value("Rock");
                    rock.append_attribute("x").set_value(i);
                    rock.append_attribute("y").set_value(j);
                }
            }
        }
    }
}

void MapGenerator::generate(const std::string& name) {
    std::string path = "maps/" + name + ".xml";
    xml_document document;
    xml_node root = document.append_child("Map");
    xml_node terrain = root.append_child("Terrain");
    generate_blank_map(terrain);
    generate_rivers(terrain, water_cells, "Agua");
    generate_rivers(terrain, lava_cells, "Lava");


    xml_node structs = root.append_child("Structs");
    generate_structs(structs);
    print_map(terrain);
    document.save_file(path.c_str());
}

MapGenerator::~MapGenerator() {
    if (output.is_open()) {
        output.close();
    }
}
