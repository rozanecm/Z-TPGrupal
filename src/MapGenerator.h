#ifndef Z_TPGRUPAL_MAPGENERATOR_H
#define Z_TPGRUPAL_MAPGENERATOR_H

#include <fstream>

class MapGenerator {
    std::ofstream output;
    int size;
public:
    MapGenerator(int size);
    ~MapGenerator();
    void generate(const std::string& name);
};


#endif //Z_TPGRUPAL_MAPGENERATOR_H
