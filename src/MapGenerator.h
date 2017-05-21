#ifndef Z_TPGRUPAL_MAPGENERATOR_H
#define Z_TPGRUPAL_MAPGENERATOR_H

#include <fstream>
#include <vector>
#include <pugixml.hpp>

/* Map generator. Randomly generates a readable .xml map file basing off the
 * passed arguments on the constructor. The maps are saved to the 'maps' folder
 * in the root directory. */

class MapGenerator {
    std::ofstream output;
    int size;
    float lava_pct;
    float water_pct;
    float terrain_var;
    int water_cells;
    int lava_cells;
public:
    MapGenerator(int size, float lava_pct, float water_pct,
                 float terrain_variance_pct);
    ~MapGenerator();

    // Generate the map, saving it to "maps/<name>.xml".
    void generate(const std::string& name);

private:
    void generate_blank_map(pugi::xml_node root_node);

    /* Generates cell_amt of cells, ordered in a river like structure, in the
     * map given by the root node. The cells are written with a "terrain"
     * attribute as children of the root node, with the value given by the
     * string 'terrain'. */
    void generate_rivers(pugi::xml_node root_node, int cell_amt,
                             const std::string &terrain);

    /* Generates river-like paths in a 'size' big square map, represented by
     * a matrix of boolean values. You can modify the RNG seed with the 'seed'
     * attribute. */
    std::vector<std::vector<bool>> generate_path(int amt, time_t seed);
};


#endif //Z_TPGRUPAL_MAPGENERATOR_H
