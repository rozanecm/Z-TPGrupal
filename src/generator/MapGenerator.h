#ifndef Z_TPGRUPAL_MAPGENERATOR_H
#define Z_TPGRUPAL_MAPGENERATOR_H

#include <fstream>
#include <vector>
#include <string>
#include <pugixml.hpp>
#include "Random.h"

// Config variables, percentages / amounts of features the map will have
#define ROCK_PCT 2
#define BRIDGE_AMT size / 20
#define RIVER_END_PCT 5
#define FORTS_AMT 4



/* Map generator. Randomly generates a readable .xml map file basing off the
 * passed arguments on the constructor. The maps are saved to the 'maps' folder
 * in the root directory. */
class MapGenerator {
    std::vector<std::vector<bool>> liquid_cells;
    std::ofstream output;
    int size;
    float lava_pct;
    float water_pct;
    int water_cells;
    int lava_cells;
    int terr;

    std::string name;
    // Random number generator
    Random r;

public:
    MapGenerator(int size, float lava_pct,
                     float water_pct, int territories);
    ~MapGenerator();

    // Generate the map, saving it to "maps/<name>.xml".
    void generate(const std::string& name);

private:
    /* Inits a map */
    void generate_blank_map(pugi::xml_node root_node);

    /* Generates cell_amt of cells, ordered in a river like structure, in the
     * map given by the root node. The cells are written with a "terrain"
     * attribute as children of the root node, with the value given by the
     * string 'terrain'. */
    std::vector<std::vector<bool>> generate_rivers(pugi::xml_node root_node,
                                                   int cell_amt,
                                                   const std::string &terrain);

    /* Generates river-like paths in a 'size' big square map, represented by
     * a matrix of boolean values. */
    void  generate_path(int amt, std::vector<std::vector<bool>>& path);

    /* Generates rocks */
    void generate_rocks(pugi::xml_node root);

    /* Generates FORTS_AMT forts in the map, placed separate from each other */
    void generate_territories(pugi::xml_node root);

    /* Generates 1 or 2 factories inside the territory delimited by the
     * specified bounds */
    void generate_factories(pugi::xml_node &territory, int min_x, int min_y, int max_x,
                                int max_y, pugi::xml_node &map);
};


#endif //Z_TPGRUPAL_MAPGENERATOR_H
