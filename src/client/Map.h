#ifndef Z_TPGRUPAL_MAP_H
#define Z_TPGRUPAL_MAP_H


#include <vector>
#include "Cell.h"
#include "Nature.h"
#include <string>

class Map {
private:
    std::vector<std::vector<Cell>> baseMap;
    std::vector<Nature> nature;

public:
    void initializeMap(unsigned int xSize, unsigned int ySize);

    void addNature(Nature nature);

    std::vector<Nature> getNature();

    void setCell(unsigned int xCoordinate,
                 unsigned int yCoordinate,
                 std::string terrainType);

    unsigned int getXSize();

    unsigned int getYSize();

    std::string getTerrainTypeAt(unsigned int xCoordinate,
                                 unsigned int yCoordinate);
};


#endif //Z_TPGRUPAL_MAP_H
