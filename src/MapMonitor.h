#ifndef Z_TPGRUPAL_MAPMONITOR_H
#define Z_TPGRUPAL_MAPMONITOR_H


#include <mutex>
#include "Map.h"

class MapMonitor {
private:
    Map &map;

    std::mutex m;

public:
    MapMonitor(Map &map);

    void setCell(unsigned int xCoordinate, unsigned int yCoordinate, std::string terrainType);

    void initializeMap(unsigned int xSize, unsigned int ySize);

    unsigned int getXSize();

    unsigned int getYSize();

    std::string getTerrainTypeAt(unsigned int xCoordinate,
                                 unsigned int yCoordinate);
};


#endif //Z_TPGRUPAL_MAPMONITOR_H
