#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include "MapGenerator.h"
#include <pugixml.hpp>
#include <random>


void print_map(pugi::xml_node& root_node) {
/* for (pugi::xml_node& row : root_node.children()) {
        for (pugi::xml_node &node : row.children()) {
            char terrain = node.attribute(TERRAIN).value()[0];
            if (terrain == 'T') {
                terrain = ' ';
            }
            std::cout << terrain << " ";
        }
        std::cout << std::endl;
    }*/
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
        liquid_cells.push_back(row);
    }
    water_cells = (int) (size * size * water_pct / 100);
    lava_cells = (int) (size * size * lava_pct / 100);
    terr = 9;
}

void MapGenerator::generate_blank_map(pugi::xml_node root_node) {
    for (int i = 0; i < size; ++i) {
        pugi::xml_node row = root_node.append_child("Row");
        for (int j = 0; j < size; ++j) {
            pugi::xml_node cell = row.append_child("Cell");
            pugi::xml_attribute attr = cell.append_attribute(TERRAIN);
            attr.set_value("Tierra");
        }
    }
}

std::vector<std::vector<bool>>
MapGenerator::generate_rivers(pugi::xml_node root_node, int cell_amt,
                                   const std::string &terrain) {
    std::vector<std::vector<bool>> map;
    generate_path(cell_amt, time(NULL), map);
    int count_y = 0;
    for (pugi::xml_node& row : root_node.children()) {
        int count_x = 0;
        for (pugi::xml_node& node : row.children()) {
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
                liquid_cells[i][j] = true;
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

    int river_x = r.generate() % size;
    int river_y = r.generate() % size;

    while (amt) {
        path[river_x][river_y] = true;

        bool found = false;

        while (!found) {
            int end = r.generate() % 100;
            if (end < RIVER_END_PCT) { // Start another river somewhere else
                river_x = r.generate() % size;
                river_y = r.generate() % size;
            }
            // Grab an adjacent tile randomly to be the next water tile
            int next = r.generate() % 4;
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

void MapGenerator::generate_rocks(pugi::xml_node root) {
    root.set_name("Structures");
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            if (!liquid_cells[i][j]) {
                int chance = r.generate() % 100;
                if (chance < ROCK_PCT) {
                    pugi::xml_node rock = root.append_child("Struct");
                    rock.append_attribute("Type").set_value("Rock");
                    rock.append_attribute("x").set_value(i);
                    rock.append_attribute("y").set_value(j);
                }
            }
        }
    }
}

/* UNFINISHED */
void MapGenerator::populate_bridge(int x, int y)  {
    pugi::xml_node bridge;
    int direction = r.generate() % 2;
    if (direction) { // horizontal
        int dir = 1;
        int offset = 0;
        bool done = false;
        while (!done) {
            pugi::xml_node child = bridge.append_child("Struct");
            child.append_attribute("Type").set_value("Bridge");
            child.append_attribute("x").set_value(x + dir * offset);
            child.append_attribute("y").set_value(y);
            offset++;
            if (x + dir * offset >= size || x + dir * offset < 0) {
                dir = -1 * dir;
                offset = 1;
            }
            done = true; // idk
        }
    }
}

/* UNFINISHED */
void MapGenerator::generate_bridges() {
    int amt = BRIDGE_AMT;
    while (amt) {
        int x = r.generate() % size;
        int y = r.generate() % size;
        if (liquid_cells[x][y]) {
        }

        amt--;
    }
}

void MapGenerator::generate(const std::string& name) {
    std::string path = "maps/" + name + ".xml";
    pugi::xml_document document;
    pugi::xml_node root = document.append_child("Map");
    pugi::xml_node terrain = root.append_child("Terrain");
    generate_blank_map(terrain);

    generate_territories(root);
    generate_rivers(terrain, water_cells, "Agua");
    generate_rivers(terrain, lava_cells, "Lava");


    pugi::xml_node structs = root.append_child("Structs");
    generate_rocks(structs);
    print_map(terrain);
    bool saved = document.save_file(path.c_str());
    if (!saved) {
        std::cout << "Error saving map to " << path << std::endl;
    }
}

MapGenerator::~MapGenerator() {
    if (output.is_open()) {
        output.close();
    }
}

void MapGenerator::generate_territories(pugi::xml_node root) {
    srand(time(NULL) * time(NULL));
    pugi::xml_node forts = root.append_child("Territories");

    double size_sqrt =  sqrt(terr);
    int offset = size / terr;
    int territories_x = (int) floor(size_sqrt);
    int territories_y = (int) ceil(size_sqrt);

    int div_x = size / territories_x;
    int div_y = size / territories_y;


    /* Choose exactly FORTS_AMT of territories to be designed as central.
     * There's one fort for each expected player in the map */
    int fort_territories[FORTS_AMT];
    for (int k = 0; k < FORTS_AMT; ++k) {
        bool found = false;
        while (!found) {
            int position = r.generate() % terr;
            bool repeat = false;
            for (int i = 0; i < k; ++i) {
                if (fort_territories[i] == position) {
                    repeat = true;
                }
            }
            if (repeat) {
                continue;
            }
            fort_territories[k] = position;
            found = true;
        }
    }

    int count = 0;
    for (int i = 0; i < territories_y; ++i) {
        for (int j = 0; j < territories_x; ++j) {
            int flag_x = div_x * j + offset +
                    r.generate() % (size / (2 * terr));
            int flag_y = div_y * i + offset +
                    r.generate() % (size / (2 * terr));

            std::string name = "Flag";
            for (int k = 0; k < FORTS_AMT; ++k) {
                if (fort_territories[k] == count) {
                    name = "Fort";
                }
            }

            pugi::xml_node flag = forts.append_child(name.c_str());
            flag.append_attribute("x").set_value(flag_x);
            flag.append_attribute("y").set_value(flag_y);

            count ++;
        }
    }

    int forts_offset = size / 4;
}
