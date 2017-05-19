#ifndef Z_TPGRUPAL_MAPGENERATOR_H
#define Z_TPGRUPAL_MAPGENERATOR_H

#include <fstream>

/* Map generator. Randomly generates a readable .xml map file basing off the
 * passed arguments on the constructor. The maps are saved to the 'maps' folder
 * in the root directory. */

class MapGenerator {
    std::ofstream output;
    int size;
public:
    MapGenerator(int size);
    ~MapGenerator();

    // Generate the map, giving it the specified name.
    void generate(const std::string& name);
};


#endif //Z_TPGRUPAL_MAPGENERATOR_H
