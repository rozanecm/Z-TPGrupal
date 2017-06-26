#include "Map.h"

#define LENIENCY 3

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

int Map::get_flag_at(int x, int y) {
    for (auto flag : flags) {
        std::pair<int, int> position = flag.second.second;
        int x_abs = abs(x - position.first);
        int y_abs = abs(y - position.second);
        if (x_abs <= LENIENCY && y_abs <= LENIENCY) {
            return flag.first;
        }
    }
    return -1;
}
