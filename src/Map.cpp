#include "Map.h"

void Map::initializeMap(unsigned int xSize, unsigned int ySize) {
    baseMap.resize(xSize);
    for (int i = 0; i < xSize; ++i){
        baseMap[i].resize(ySize);
    }
}

void Map::setCell(unsigned int xCoordinate, unsigned int yCoordinate,
                  std::string terrainType) {
    baseMap.at(xCoordinate).at(yCoordinate).assignTerrainType(terrainType);
}

unsigned int Map::getXSize() {
    return baseMap.size();
}

unsigned int Map::getYSize() {
    return baseMap[0].size();
}

std::string
Map::getTerrainTypeAt(unsigned int xCoordinate, unsigned int yCoordinate) {
    return baseMap.at(xCoordinate).at(yCoordinate).getTerrainType();
}
