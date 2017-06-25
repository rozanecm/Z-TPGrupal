#include "Map.h"

void Map::initializeMap(unsigned int xSize, unsigned int ySize) {
    baseMap.resize(xSize);
    for (int i = 0; i < xSize; ++i) {
        baseMap[i].resize(ySize);
    }
}

void Map::setCell(unsigned int xCoordinate, unsigned int yCoordinate,
                  std::string terrainType) {
    baseMap.at(xCoordinate).at(yCoordinate).assignTerrainType(terrainType);
}

unsigned int Map::getXSize() {
    return (unsigned int) baseMap.size();
}

unsigned int Map::getYSize() {
    if (!baseMap.size()) {
        return 0;
    }
    return (unsigned int) baseMap[0].size();
}

std::string
Map::getTerrainTypeAt(unsigned int xCoordinate, unsigned int yCoordinate) {
    return baseMap.at(xCoordinate).at(yCoordinate).getTerrainType();
}

void Map::addNature(Nature nature) {
    this->nature.emplace_back(nature);
}

std::vector<Nature> Map::getNature() {
    return nature;
}

void Map::clear() {
    baseMap.clear();
    nature.clear();
    flags.clear();
}

void Map::update_territory(const int &id, const TeamEnum &team, const int &x,
                           const int &y) {
    flags[id] = {team,{x,y}};
}

std::map<int, std::pair<TeamEnum, std::pair<unsigned int, unsigned int>>>
Map::getFlags() {
    return flags;
}
