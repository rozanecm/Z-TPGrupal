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

    // Generate the map, giving it the specified name.
    void generate(const std::string& name);

private:
    void generate_blank_map(pugi::xml_node root_node);
    void generate_rivers(pugi::xml_node root_node);

    std::vector<std::vector<bool>> generate_path(int amt);
};


#endif //Z_TPGRUPAL_MAPGENERATOR_H
