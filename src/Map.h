#ifndef Z_TPGRUPAL_MAP_H
#define Z_TPGRUPAL_MAP_H


#include <vector>
#include "Cell.h"

class Map {
private:
    std::vector<std::vector<Cell>> baseMap;

public:
    void initializeMap(unsigned int xSize, unsigned int ySize);

    void setCell(unsigned int xCoordinate, unsigned int yCoordinate, std::string terrainType);
};


#endif //Z_TPGRUPAL_MAP_H
