#include <fstream>
#include <vector>
#include <iostream>
#include "MapGenerator.h"
#include "pugixml.hpp"

#define RIVER_END_PCT 10
#define TERRAIN "terrain"

using namespace pugi;

void print_map(xml_node& root_node) {
    for(xml_node& row : root_node.children()) {
        for (xml_node &node : row.children()) {
            std::cout << node.attribute(TERRAIN).value() << " ";
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
    std::vector<std::vector<bool>> water = generate_path(water_cells);
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

    std::vector<std::vector<bool>> lava = generate_path(lava_cells);
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

std::vector<std::vector<bool>> MapGenerator::generate_path(int amt) {
    srand((unsigned int) time(NULL));
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
        while (!found) {
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

            if (next_x >= size or next_x < 0 or next_y >= size or next_y < 0) {
                continue;
            }

            if (!path[next_x][next_y]) {
                found = true;
                amt--;
                river_x = next_x;
                river_y = next_y;
            }

            int end = rand() % 100;
            if (end < RIVER_END_PCT) { // Start another river somewhere else
                river_x = rand() % size;
                river_y = rand() % size;
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
