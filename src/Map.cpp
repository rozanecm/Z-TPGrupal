#include "Map.h"

void Map::setCell(unsigned int xCoordinate, unsigned int yCoordinate,
                  std::string terrainType) {
    baseMap.at(xCoordinate).at(yCoordinate).assignTerrainType(terrainType);
}

void Map::initializeMap(unsigned int xSize, unsigned int ySize) {
    baseMap.resize(xSize);
    for (int i = 0; i < xSize; ++i){
        baseMap[i].resize(ySize);
    }
}
