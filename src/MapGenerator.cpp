#include <fstream>
#include <vector>
#include <iostream>
#include "MapGenerator.h"
#include <pugixml.hpp>

#define RIVER_END_PCT 10
#define TERRAIN "terrain"

#define EAST 1
#define SOUTH 2
#define WEST -1
#define NORTH -2

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

void MapGenerator::generate_rivers(pugi::xml_node root_node) {
    std::vector<std::vector<bool>> water = generate_path(water_cells, time(NULL));
    int count_y = 0;
    for(xml_node& row : root_node.children()) {
        int count_x = 0;
        for(xml_node& node : row.children()) {
            if (water[count_x][count_y]) {
                node.attribute(TERRAIN).set_value("Agua");
            }
            count_x++;
        }
        count_y++;
    }

    std::vector<std::vector<bool>> lava = generate_path(lava_cells, 2*time(NULL));
    count_y = 0;
    for(xml_node& row : root_node.children()) {
        int count_x = 0;
        for(xml_node& node : row.children()) {
            if (lava[count_x][count_y]) {
                node.attribute(TERRAIN).set_value("Lava");
            }
            count_x++;
        }
        count_y++;
    }
}

bool MapGenerator::in_bounds(std::vector<std::vector<bool>> &vec,
                             int x, int y) {
    if (x > 0 && y > 0 && x < size && y < size) {
        return vec[x][y];
    }
    return false;
}

std::vector<std::vector<bool>> MapGenerator::generate_path(int amt, time_t seed) {
    srand((unsigned int) seed);
    std::vector<std::vector<bool>> path;
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

        int prev_dir = 0;
        while (!found) {
            int end = rand() % 100;
            if (end < RIVER_END_PCT) { // Start another river somewhere else
                river_x = rand() % size;
                river_y = rand() % size;
            }
            // Grab an adjacent tile randomly to be the next water tile
            int next = rand() % 4;
            int next_dir = 0;
            int next_x, next_y;
            if (next == 0) {
                next_x = 1;
                next_y = 0;
                next_dir = EAST;
            } else if (next == 1) {
                next_x = 0;
                next_y = -1;
                next_dir = SOUTH;
            } else if (next == 2) {
                next_x = -1;
                next_y = 0;
                next_dir = WEST;
            } else {
                next_x = 0;
                next_y = 1;
                next_dir = NORTH;
            }
            next_x += river_x;
            next_y += river_y;


            // Check for out of bounds
            if (!(next_x > 0 && next_y > 0 && next_x < size && next_y < size)) {
                continue;
            }

            if (next_dir == -1 * prev_dir) {
                continue;
            }

            if (!path[next_x][next_y]) {
                prev_dir = -1 * next_dir;
                found = true;
                amt--;
                river_x = next_x;
                river_y = next_y;
            }
        }
    }
    return path;
}


void MapGenerator::generate(const std::string& name) {
    std::string path = "maps/" + name + ".xml";
    xml_document document;
    xml_node root = document.append_child("Map");
    generate_blank_map(root);
    generate_rivers(root);
    print_map(root);
    document.save_file(path.c_str());
}

MapGenerator::~MapGenerator() {
    if (output.is_open()) {
        output.close();
    }
}
